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
namespace platinum
{

    CameraSample Sampler::GetCameraSample(const Vector2i &p_raster, Filter *flter)
    {
        CameraSample cs;
        cs.p_film = (Vector2f)p_raster + Get2D();
        return cs;
    }

    void Sampler::StartPixel(const Vector2i &p)
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
        _sampleArray2D.push_back(std::vector<Vector2i>(n * _samplesPerPixel));
    }

    const float *Sampler::Get1DArray(int n)
    {
        if (_array1DOffset == _sampleArray1D.size())
            return nullptr;
        CHECK_EQ(_samples1DArraySizes[_array1DOffset], n);
        CHECK_LT(_currentPixelSampleIndex, _samplesPerPixel);
        return &_sampleArray1D[_array1DOffset++][_currentPixelSampleIndex * n];
    }

    const Vector2i *Sampler::Get2DArray(int n)
    {
        if (_array2DOffset == _sampleArray2D.size())
            return nullptr;
        CHECK_EQ(_samples2DArraySizes[_array2DOffset], n);
        CHECK_LT(_currentPixelSampleIndex, _samplesPerPixel);
        return &_sampleArray2D[_array2DOffset++][_currentPixelSampleIndex * n];
    }

}