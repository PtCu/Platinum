

#ifndef SAMPLER_RANDOM_SAMPLER_H_
#define SAMPLER_RANDOM_SAMPLER_H_

#include <core/sampler.h>
#include <math/rand.h>
namespace platinum
{
    class RandomSampler final : public Sampler
    {
    public:
        RandomSampler(const PropertyTree &);

        RandomSampler(int ns);

        virtual void StartPixel(const Vector2i &) override;

        virtual float Get1D() override;
        virtual Vector2f Get2D() override;

        //Clone() 方法生成一个初始采样器的新实例，供渲染线程使用
        virtual std::unique_ptr<Sampler> Clone(int seed) override;

        virtual std::string ToString() const { return "RandomSampler"; }
    };
}

#endif