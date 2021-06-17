#ifndef CORE_INTEGRATOR_H_
#define CORE_INTEGRATOR_H_

#include "platinum.h"
#include "math_utils.h"
#include "sampler.h"
#include "camera.h"
#include "hitable.h"
#include "rtti.h"

namespace platinum{

class Integrator : public Object
{
public:
    typedef std::shared_ptr<Integrator> ptr;

    virtual ~Integrator() = default;

    virtual void preprocess(const Scene &scene) = 0;
    virtual void render(const Scene &scene) = 0;

    virtual ClassType getClassType() const override { return ClassType::IntegratorType; }
};

class SamplerIntegrator : public Integrator
{
public:
    typedef std::shared_ptr<SamplerIntegrator> ptr;

    // SamplerIntegrator Public Methods
    SamplerIntegrator(Camera::ptr camera, Sampler::ptr sampler)
        : m_camera(camera), m_sampler(sampler) {}

    virtual void preprocess(const Scene &scene) override {}

    virtual void render(const Scene &scene) override;

    virtual Spectrum Li(const Ray &ray, const Scene &scene,
                        Sampler &sampler, MemoryArena &arena, int depth = 0) const = 0;

    Spectrum specularReflect(const Ray &ray, const SurfaceInteraction &isect,
                             const Scene &scene, Sampler &sampler, MemoryArena &arena, int depth) const;

    Spectrum specularTransmit(const Ray &ray, const SurfaceInteraction &isect,
                              const Scene &scene, Sampler &sampler, MemoryArena &arena, int depth) const;

protected:
    Camera::ptr m_camera;
    Sampler::ptr m_sampler;
};

Spectrum uiformSampleAllLights(const Interaction &it, const Scene &scene,
                               MemoryArena &arena, Sampler &sampler, const std::vector<int> &nLightSamples);

Spectrum uniformSampleOneLight(const Interaction &it, const Scene &scene,
                               MemoryArena &arena, Sampler &sampler, const Distribution1D *lightDistrib);

Spectrum estimateDirect(const Interaction &it, const Vector2f &uShading, const Light &light,
                        const Vector2f &uLight, const Scene &scene, Sampler &sampler, MemoryArena &arena, bool specular = false);

}

#endif