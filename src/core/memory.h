#ifndef CORE_MEMORY_H_
#define CORE_MEMORY_H_

#include <list>
#include <algorithm>
#include <cstddef>
#include <core/utilities.h>

namespace platinum
{

// #define REGISTER_ARENA                                        \
//     static void *operator new(size_t size, MemoryArena arena) \
//     {                                                         \
//         return arena.Alloc(size);                             \
//     }                                                         \
//     static void operator delete(void *p, size_t size) {}
    //无需operator delete，每次循环由arean统一清空所分配的内存

#define ARENA_ALLOC(arena, Type) new ((arena).Alloc(sizeof(Type))) Type

    void *AllocAligned(size_t size);

    template <typename T>
    T *AllocAligned(size_t count)
    {
        return (T *)AllocAligned(count * sizeof(T));
    }

    void FreeAligned(void *);

    class MemoryArena
    {

    public:
        MemoryArena(size_t blockSize = 262144) : blockSize(blockSize) {}
        virtual ~MemoryArena()
        {
            FreeAligned(currentBlock);
            for (auto &block : usedBlocks)
                FreeAligned(block.second);
            for (auto &block : availableBlocks)
                FreeAligned(block.second);
        }

        void *Alloc(size_t nBytes)
        {
            const int align = 16;
            //Round up nBytes to minimum machine alignment
            nBytes = (nBytes + align - 1) & ~(align - 1);

            //Current memory is not enough. Require new memory.
            if (currentBlockPos + nBytes > currentAllocSize)
            {
                // Add current block to _usedBlocks_ list
                if (currentBlock)
                {
                    usedBlocks.push_back(std::make_pair(currentAllocSize, currentBlock));
                    currentBlock = nullptr;
                    currentAllocSize = 0;
                }
                // Get new block of memory for _MemoryArena_
                // Try to get memory block from _availableBlocks_
                // The allocation routine first checks to see
                // if there are any already allocated free blocks in availableBlocks.
                for (auto iter = availableBlocks.begin();
                     iter != availableBlocks.end(); ++iter)
                {
                    if (iter->first >= nBytes)
                    {
                        currentAllocSize = iter->first;
                        currentBlock = iter->second;
                        availableBlocks.erase(iter);
                        break;
                    }
                }
                if (!currentBlock)
                {
                    currentAllocSize = std::max(nBytes, blockSize);
                    currentBlock = AllocAligned<uint8_t>(currentAllocSize);
                }
                currentBlockPos = 0;
            }
            //Slice a chunk for the requester.
            void *ret = currentBlock + currentBlockPos;
            currentBlockPos += nBytes;
            return ret;
        }

        template <typename T>
        T *Alloc(size_t n = 1, bool runContructor = true)
        {
            T *ret = (T *)Alloc(n * sizeof(T));
        }

        void Reset()
        {
            currentBlockPos = 0;
            availableBlocks.splice(availableBlocks.begin(), usedBlocks);
        }

        size_t TotalAllocated() const
        {
            size_t total = currentAllocSize;
            for (const auto &alloc : usedBlocks)
                total += alloc.first;
            for (const auto &alloc : availableBlocks)
                total += alloc.first;
            return total;
        }

    private:
        MemoryArena(const MemoryArena &) = delete;
        MemoryArena &operator=(const MemoryArena &) = delete;
        const size_t blockSize;
        size_t currentBlockPos = 0, currentAllocSize = 0;
        //currentBlock是当前内存块的头指针。
        uint8_t *currentBlock = nullptr;
        //usedBlocks是已经分配过的内存块集合，availableBlocks是由usedBlocks回收来的内存块集合，当作战备池
        //每一次Reset()操作都会使得usedBlocks中分配的内存回收进availableBlocks中进行备用
        std::list<std::pair<size_t, uint8_t *>> usedBlocks, availableBlocks;
    };
}
#endif //
