

#include <core/sampler.h>
#include <core/camera.h>
namespace platinum
{

    Sampler::Sampler(const PropertyTree &root) : _samplesPerPixel(root.Get<int64_t>("SPP"))
    {
    }
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
        _sampleArray2D.push_back(std::vector<Vector2f>(n * _samplesPerPixel));
    }

    const float *Sampler::Get1DArray(int n)
    {
        if (_array1DOffset == _sampleArray1D.size())
            return nullptr;
        CHECK_EQ(_samples1DArraySizes[_array1DOffset], n);
        CHECK_LT(_currentPixelSampleIndex, _samplesPerPixel);
        return &_sampleArray1D[_array1DOffset++][_currentPixelSampleIndex * n];
    }

    const Vector2f *Sampler::Get2DArray(int n)
    {
        if (_array2DOffset == _sampleArray2D.size())
            return nullptr;
        CHECK_EQ(_samples2DArraySizes[_array2DOffset], n);
        CHECK_LT(_currentPixelSampleIndex, _samplesPerPixel);
        return &_sampleArray2D[_array2DOffset++][_currentPixelSampleIndex * n];
    }

}