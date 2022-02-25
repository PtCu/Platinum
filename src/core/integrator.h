

#ifndef CORE_INTEGREATOR_H_
#define CORE_INTEGREATOR_H_

#include <core/utilities.h>
#include <core/scene.h>
#include <core/camera.h>
#include <core/film.h>
#include <core/sampler.h>
#include <core/object.h>

namespace platinum
{

    class Integrator : public Object
    {
    public:
        virtual void Render(const Scene &scene) = 0;
    };

    class SamplerIntegrator : public Integrator
    {
    public:
        SamplerIntegrator(UPtr<Camera> camera, UPtr<Sampler> sampler)
            : _camera(std::move(camera)), _sampler(std::move(sampler)) {}

        virtual void Render(const Scene &scene);

        virtual void Preprocess(const Scene &scene, Sampler &sampler) = 0;

        void SetSampler(UPtr<Sampler> sampler)
        {
            _sampler = std::move(sampler);
        }

        void SetCamera(UPtr<Camera> camera) { _camera = std::move(camera); }

    protected:
        /**
         * @brief  Li() 方法计算有多少光照量沿着该 Ray 到达成像平面，
         *          并把光照量（radiance）保存在 Film 内
         * 
         * @param scene 
         * @param ray 
         * @param sampler 
         * @param depth 
         * @return Spectrum 
         */
        virtual Spectrum Li(const Scene &scene, const Ray &ray, Sampler &sampler, MemoryArena &arena, int depth = 0) const = 0;

        // 高光反射
        Spectrum SpecularReflect(const Ray &ray, const SurfaceInteraction &inter, const Scene &scene, Sampler &sampler, MemoryArena &arena, int depth) const;

        // 高光透射
        Spectrum SpecularTransmit(const Ray &ray, const SurfaceInteraction &inter, const Scene &scene, Sampler &sampler, MemoryArena &arena, int depth) const;

    protected:
        UPtr<Camera> _camera;
        UPtr<Sampler> _sampler;
    };

    Spectrum UniformSampleAllLights(const Interaction &it, const Scene &scene, MemoryArena &arena, Sampler &sampler,
                                    const std::vector<int> &nLightSamples);
    Spectrum UniformSampleOneLight(const Interaction &it, const Scene &scene, MemoryArena &arena,
                                   Sampler &sampler,
                                   const Distribution1D *lightDistrib = nullptr);
    Spectrum EstimateDirect(const Interaction &it, const Vector2f &uShading,
                            const Light &light, const Vector2f &uLight,
                            const Scene &scene, Sampler &sampler,
                            MemoryArena &arena,
                            bool specular = false);
}

#endif