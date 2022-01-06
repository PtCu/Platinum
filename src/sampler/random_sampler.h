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

#ifndef SAMPLER_RANDOM_SAMPLER_H_
#define SAMPLER_RANDOM_SAMPLER_H_

#include <core/sampler.h>
#include <math/rand.h>
namespace platinum {
    class RandomSampler final :public Sampler
    {
    public:
        RandomSampler(int ns);
        virtual void StartPixel(const glm::ivec2&) override;

        virtual float Get1D() override;
        virtual glm::vec2 Get2D() override;

        //Clone() 方法生成一个初始采样器的新实例，供渲染线程使用
        virtual std::unique_ptr<Sampler> Clone(int seed) override;
    };
}

#endif