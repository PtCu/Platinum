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

#include <sampler/random_sampler.h>

namespace platinum {

    RandomSampler::RandomSampler(int ns) : Sampler(ns) {}

    float RandomSampler::Get1D()
    {
        return Random::UniformFloat();
    }

    glm::vec2 RandomSampler::Get2D()
    {
        return Random::UniformDisk();
    }

    std::unique_ptr<Sampler> RandomSampler::Clone(int seed)
    {
        return std::unique_ptr<Sampler>(new RandomSampler(*this));
    }

    void RandomSampler::StartPixel(const glm::ivec2& p)
    {
        for (size_t i = 0; i < _sampleArray1D.size(); ++i)
            for (size_t j = 0; j < _sampleArray1D[i].size(); ++j)
                _sampleArray1D[i][j] = Random::UniformFloat();

        for (size_t i = 0; i < _sampleArray2D.size(); ++i)
            for (size_t j = 0; j < _sampleArray2D[i].size(); ++j)
                _sampleArray2D[i][j] = Random::UniformDisk();

        Sampler::StartPixel(p);
    }

}