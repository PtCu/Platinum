// Copyright 2022 ptcup
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CORE_PARRALLEL_H_
#define CORE_PARRALLEL_H_

#include <core/utilities.h>

#include <mutex>
#include <atomic>
#include <thread>
#include <functional>
#include <condition_variable>

namespace platinum
{
	class AtomicFloat
	{
	public:
		explicit AtomicFloat(float v = 0) { bits = floatToBits(v); }

		operator float() const { return bitsToFloat(bits); }

		float operator=(float v)
		{
			bits = floatToBits(v);
			return v;
		}

		void add(float v)
		{
#ifdef AURORA_DOUBLE_AS_FLOAT
			uint64_t oldBits = bits, newBits;
#else
			uint32_t oldBits = bits, newBits;
#endif
			do
			{
				newBits = floatToBits(bitsToFloat(oldBits) + v);
			} while (!bits.compare_exchange_weak(oldBits, newBits));
		}

	private:
#ifdef AURORA_DOUBLE_AS_FLOAT
		std::atomic<uint64_t> bits;
#else
		std::atomic<uint32_t> bits;
#endif
	};

	// Simple one-use barrier; ensures that multiple threads all reach a
	// particular point of execution before allowing any of them to proceed
	// past it.
	//
	// Note: this should be heap allocated and managed with a shared_ptr, where
	// all threads that use it are passed the shared_ptr. This ensures that
	// memory for the Barrier won't be freed until all threads have
	// successfully cleared it.
	class Barrier
	{
	public:
		Barrier(int count) : m_count(count) { CHECK_GT(count, 0); }
		~Barrier() { CHECK_EQ(m_count, 0); }
		void wait();

	private:
		std::mutex m_mutex;
		std::condition_variable m_cv;
		int m_count;
	};

	//Execution policy tag.
	enum class ExecutionPolicy
	{
		SERIAL,
		PARALLEL
	};

	inline int numSystemCores() { return glm::max(1u, std::thread::hardware_concurrency()); }

	class ParallelUtils
	{
	public:
		//Parallel loop for parallel tiling rendering
		template <typename Function>
		static void parallelFor(size_t start, size_t end, const Function &func, ExecutionPolicy policy)
		{
			if (start > end)
				return;
			if (policy == ExecutionPolicy::PARALLEL)
			{
				ParallelUtils::parallel_for_seize(start, end, func);
			}
			else
			{
				for (auto i = start; i < end; ++i)
					func(i);
			}
		}

	private:
		template <typename Callable>
		static void parallel_for(size_t start, size_t end, Callable function)
		{
			DCHECK(start < end);
			//Note: this parallel_for split the task in a simple averaging manner
			//      which is inefficient for inbalance task among threads
			const int n_threads = numSystemCores();
			const size_t n_task = end - start;

			const int n_max_tasks_per_thread = (n_task / n_threads) + (n_task % n_threads == 0 ? 0 : 1);
			const int n_lacking_tasks = n_max_tasks_per_thread * n_threads - n_task;

			auto inner_loop = [&](const int thread_index)
			{
				const int n_lacking_tasks_so_far = std::max(thread_index - n_threads + n_lacking_tasks, 0);
				const int inclusive_start_index = thread_index * n_max_tasks_per_thread - n_lacking_tasks_so_far;
				const int exclusive_end_index = inclusive_start_index + n_max_tasks_per_thread - (thread_index - n_threads + n_lacking_tasks >= 0 ? 1 : 0);

				for (int k = inclusive_start_index; k < exclusive_end_index; ++k)
				{
					function(k);
				}
			};
			std::vector<std::thread> threads;
			for (int j = 0; j < n_threads; ++j)
			{
				threads.push_back(std::thread(inner_loop, j));
			}
			for (auto &t : threads)
			{
				t.join();
			}
		}

		template <typename Callable>
		static void parallel_for_seize(size_t start, size_t end, Callable func)
		{
			DCHECK(start < end);
			//Note: this parallel_for assign the task to thread by atomic
			//      opertion over task index which is more efficient in general case

			const int n_threads = numSystemCores();
			const size_t n_task = end - start;

			std::atomic<size_t> task_index(start);
			auto inner_loop = [&](const int thread_index)
			{
				size_t index;
				while ((index = task_index.fetch_add(1)) < end)
				{
					func(index);
				}
			};
			std::vector<std::thread> threads;
			for (int j = 0; j < n_threads; ++j)
			{
				threads.push_back(std::thread(inner_loop, j));
			}
			for (auto &t : threads)
			{
				t.join();
			}
		}
	};

}

#endif