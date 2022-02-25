

#include <sampler/random_sampler.h>

namespace platinum
{
    REGISTER_CLASS(RandomSampler, "Random");

    RandomSampler::RandomSampler(const PropertyTree &root) : Sampler(root)
    {
    }

    RandomSampler::RandomSampler(int ns) : Sampler(ns) {}

    float RandomSampler::Get1D()
    {
        return Rng::UniformFloat();
    }

    Vector2f RandomSampler::Get2D()
    {
        return Vector2f(Rng::UniformFloat(), Rng::UniformFloat());
    }

    std::unique_ptr<Sampler> RandomSampler::Clone(int seed)
    {
        return std::unique_ptr<Sampler>(new RandomSampler(*this));
    }

    void RandomSampler::StartPixel(const Vector2i &p)
    {
        for (size_t i = 0; i < _sampleArray1D.size(); ++i)
            for (size_t j = 0; j < _sampleArray1D[i].size(); ++j)
                _sampleArray1D[i][j] = Rng::UniformFloat();

        for (size_t i = 0; i < _sampleArray2D.size(); ++i)
            for (size_t j = 0; j < _sampleArray2D[i].size(); ++j)
                _sampleArray2D[i][j] = Rng::UniformDisk();

        Sampler::StartPixel(p);
    }

}