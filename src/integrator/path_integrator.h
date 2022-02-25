

#ifndef INTEGRATOR_PATH_INTEGRATOR_H_
#define INTEGRATOR_PATH_INTEGRATOR_H_

#include <core/integrator.h>

namespace platinum
{

    class PathIntegrator : public SamplerIntegrator
    {
    public:
        PathIntegrator(const PropertyTree &root);

        PathIntegrator(UPtr<Camera> camera, UPtr<Sampler> sampler, int max_depth, float rr_threshold = 0.8)
            : SamplerIntegrator(std::move(camera), std::move(sampler)), _max_depth(max_depth), _rr_threshold(rr_threshold) {}

        virtual std::string ToString() const { return "PathIntegrator"; }

        virtual void Preprocess(const Scene &scene, Sampler &sampler) override;

    protected:
        virtual Spectrum Li(const Scene &scene, const Ray &ray, Sampler &sampler, MemoryArena &arena, int depth) const override;
        const int _max_depth;
        float _rr_threshold;
        std::string _light_sample_strategy;
        std::unique_ptr<LightDistribution> _light_distribution;
    };
}
#endif