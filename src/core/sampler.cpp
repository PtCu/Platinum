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

#include <core/sampler.h>
#include <core/camera.h>
namespace platinum {

    CameraSample Sampler::GetCameraSample(const glm::ivec2& p_raster,Filter* flter)
    {
        CameraSample cs;
        cs.p_film = (glm::vec2)p_raster + Get2D();
        return cs;
    }

    void Sampler::StartPixel(const glm::ivec2& p)
    {
        _currentPixel = p;
        _currentPixelSampleIndex = 0;
        // Reset array offsets for next pixel sample
        _array1DOffset = _array2DOffset = 0;
    }

    bool Sampler::StartNextSample()
    {
        // Reset array offsets for next pixel sample
        _array1DOffset = _array2DOffset = 0;
        return ++_currentPixelSampleIndex < _samplesPerPixel;
    }

    bool Sampler::SetSampleNumber(int64_t sampleNum)
    {
        // Reset array offsets for next pixel sample
        _array1DOffset = _array2DOffset = 0;
        _currentPixelSampleIndex = sampleNum;
        return _currentPixelSampleIndex < _samplesPerPixel;
    }

    void Sampler::Request1DArray(int n)
    {
        CHECK_EQ(RoundCount(n), n);
        _samples1DArraySizes.push_back(n);
        _sampleArray1D.push_back(std::vector<float>(n * _samplesPerPixel));
    }

    void Sampler::Request2DArray(int n)
    {
        CHECK_EQ(RoundCount(n), n);
        _samples2DArraySizes.push_back(n);
        _sampleArray2D.push_back(std::vector<glm::ivec2>(n * _samplesPerPixel));
    }

    const float* Sampler::Get1DArray(int n)
    {
        if (_array1DOffset == _sampleArray1D.size())
            return nullptr;
        CHECK_EQ(_samples1DArraySizes[_array1DOffset], n);
        CHECK_LT(_currentPixelSampleIndex, _samplesPerPixel);
        return &_sampleArray1D[_array1DOffset++][_currentPixelSampleIndex * n];
    }

    const glm::ivec2* Sampler::Get2DArray(int n)
    {
        if (_array2DOffset == _sampleArray2D.size())
            return nullptr;
        CHECK_EQ(_samples2DArraySizes[_array2DOffset], n);
        CHECK_LT(_currentPixelSampleIndex, _samplesPerPixel);
        return &_sampleArray2D[_array2DOffset++][_currentPixelSampleIndex * n];
    }

    glm::vec3 UniformSampleHemisphere(const glm::vec2& u)
    {
        float z = u[0];
        float r = glm::sqrt(glm::max((float)0, (float)1. - z * z));
        float phi = 2 * Pi * u[1];
        return glm::vec3(r * glm::cos(phi), r * glm::sin(phi), z);
    }

    float UniformHemispherePdf() { return Inv2Pi; }

    glm::vec3 UniformSampleSphere(const glm::vec2& u)
    {
        float z = 1 - 2 * u[0];
        float r = glm::sqrt(glm::max((float)0, (float)1 - z * z));
        float phi = 2 * Pi * u[1];
        return glm::vec3(r * glm::cos(phi), r * glm::sin(phi), z);
    }

    float UniformSpherePdf() { return Inv4Pi; }

    float UniformConePdf(float cosThetaMax) { return 1 / (2 * Pi * (1 - cosThetaMax)); }

    glm::vec2 ConcentricSampleDisk(const glm::vec2& u)
    {
        // Map uniform random numbers to $[-1,1]^2$
        glm::vec2 uOffset = 2.f * u - glm::vec2(1, 1);

        // Handle degeneracy at the origin
        if (uOffset.x == 0 && uOffset.y == 0)
            return glm::vec2(0, 0);

        // Apply concentric mapping to point
        float theta, r;
        if (glm::abs(uOffset.x) > glm::abs(uOffset.y))
        {
            r = uOffset.x;
            theta = PiOver4 * (uOffset.y / uOffset.x);
        }
        else
        {
            r = uOffset.y;
            theta = PiOver2 - PiOver4 * (uOffset.x / uOffset.y);
        }
        return r * glm::vec2(glm::cos(theta), glm::sin(theta));
    }

    glm::vec2 UniformSampleTriangle(const glm::vec2& u)
    {
        float su0 = glm::sqrt(u[0]);
        return glm::vec2(1 - su0, u[1] * su0);
    }

}