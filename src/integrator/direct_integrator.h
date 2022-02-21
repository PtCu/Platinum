#ifndef INTEGRATOR_DIRECT_INTEGRATOR_H_
#define INTEGRATOR_DIRECT_INTEGRATOR_H_

#include <core/integrator.h>
#include <core/bsdf.h>
namespace platinum
{
    enum class LightStrategy
    {
        UniformSampleAll,
        UniformSampleOne
    };

    class DirectIntegrator final : public SamplerIntegrator
    {
    public:
        DirectIntegrator(const PropertyTree &node);
        DirectIntegrator(UPtr<Camera> camera, UPtr<Sampler> sampler, int max_depth)
            : SamplerIntegrator(std::move(camera), std::move(sampler)), _max_depth(max_depth) {}
        std::string ToString() const { return "DirectIntegrator"; }
       virtual void Preprocess(const Scene &scene, Sampler &sampler)override;

    protected:
        virtual Spectrum Li(const Scene &scene, const Ray &ray, Sampler &sampler, MemoryArena &arena, int depth) const override;

    private:
        LightStrategy _strategy;
        const int _max_depth;
        std::vector<int> _n_light_samples;
    };
}

#endif