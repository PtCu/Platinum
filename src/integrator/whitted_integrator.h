

#ifndef INTEGRATOR_WHITTED_INTEGRATOR_H_
#define INTEGRATOR_WHITTED_INTEGRATOR_H_

#include <core/integrator.h>
#include <core/bsdf.h>
namespace platinum
{

    class WhittedIntegrator final : public SamplerIntegrator
    {
    public:
        WhittedIntegrator(const PropertyTree &node);
        WhittedIntegrator(UPtr<Camera> camera, UPtr<Sampler> sampler, int max_depth)
            : SamplerIntegrator(std::move(camera), std::move(sampler)), _max_depth(max_depth) {}
        virtual std::string ToString() const { return "WhittedIntegrator"; }
        virtual void Preprocess(const Scene &scene, Sampler &sampler) override {}

    protected:
        virtual Spectrum Li(const Scene &scene, const Ray &ray, Sampler &sampler, MemoryArena &arena, int depth) const override;

    private:
        const int _max_depth;
    };
}

#endif