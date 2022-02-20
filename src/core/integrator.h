// Copyright 2021 ptcup
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

        void SetSampler(UPtr<Sampler> sampler) { _sampler = std::move(sampler); }

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
        virtual Spectrum Li(const Scene &scene, const Ray &ray, Sampler &sampler, int depth = 0) const = 0;

        // 高光反射
        Spectrum SpecularReflect(const Ray &ray, const SurfaceInteraction &inter, const Scene &scene, Sampler &sampler, int depth) const;

        // 高光透射
        Spectrum SpecularTransmit(const Ray &ray, const SurfaceInteraction &inter, const Scene &scene, Sampler &sampler, int depth) const;

    protected:
        UPtr<Camera> _camera;
        UPtr<Sampler> _sampler;
    };

    Spectrum UniformSampleAllLights(const Interaction &it, const Scene &scene,
                                    Sampler &sampler,
                                    const std::vector<int> &nLightSamples,
                                    bool handleMedia = false);
    Spectrum UniformSampleOneLight(const Interaction &it, const Scene &scene,
                                   Sampler &sampler,
                                   bool handleMedia = false,
                                   const Distribution1D *lightDistrib = nullptr);
    Spectrum EstimateDirect(const Interaction &it, const Vector2f &uShading,
                            const Light &light, const Vector2f &uLight,
                            const Scene &scene, Sampler &sampler,
                            bool handleMedia = false,
                            bool specular = false);
}

#endif