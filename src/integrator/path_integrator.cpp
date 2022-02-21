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

#include "path_integrator.h"
#include <core/bsdf.h>

namespace platinum
{

    REGISTER_CLASS(PathIntegrator, "Path");

    PathIntegrator::PathIntegrator(const PropertyTree &root)
        : SamplerIntegrator(nullptr, nullptr), _max_depth(root.Get<int>("Depth"))
    {
        _sampler = UPtr<Sampler>(static_cast<Sampler *>(ObjectFactory::CreateInstance(root.Get<std::string>("Sampler.Type"), root.GetChild("Sampler"))));

        _camera = UPtr<Camera>(static_cast<Camera *>(ObjectFactory::CreateInstance(root.Get<std::string>("Camera.Type"), root.GetChild("Camera"))));

        _light_sample_strategy = root.Get<std::string>("Strategy","spatial");
    }
    void PathIntegrator::Preprocess(const Scene &scene, Sampler &sampler)
    {
        _light_distribution = createLightSampleDistribution(_light_sample_strategy, scene);
    }

    Spectrum PathIntegrator::Li(const Scene &scene, const Ray &r, Sampler &sampler, MemoryArena &arena, int depth) const
    {
        Spectrum L(0.f), beta(1.f);
        Ray ray(r);

        bool specular_bounce = false;
        int bounces;
        float eta_scale = 1.f;

        for (bounces = 0;; ++bounces)
        {
            SurfaceInteraction isect;
            bool hit = scene.Hit(ray, isect);

            if (bounces == 0 || specular_bounce)
            {
                if (hit)
                {
                    L += beta * isect.Le(-ray._direction);
                }
                else
                {
                    for (const auto &light : scene._infinite_lights)
                        L += beta * light->Le(ray);
                }
            }
            if (!hit || bounces >= _max_depth)
                break;
            isect.ComputeScatteringFunctions(ray, arena);

            if (!isect._bsdf)
            {
                ray = isect.SpawnRay(ray._direction);
                --bounces;
                continue;
            }
            const Distribution1D *distrib = _light_distribution->lookup(isect.p);

            if (isect._bsdf->NumComponents(BxDFType((int)BxDFType::BSDF_ALL & ~(int)BxDFType::BSDF_SPECULAR)) > 0)
            {
                Spectrum Ld = beta * UniformSampleOneLight(isect, scene, arena, sampler, distrib);
                CHECK_GE(Ld.y(), 0.f);
                L += Ld;
            }

            Vector3f wo = -ray._direction, wi;
            float pdf;
            BxDFType flags;
            Spectrum f = isect._bsdf->SampleF(wo, wi, sampler.Get2D(), pdf, flags, BxDFType::BSDF_ALL);

            if (f.isBlack() || pdf == 0.f)
                break;

            beta *= f * glm::abs(glm::dot(wi, isect.n)) / pdf;
            CHECK_GE(beta.y(), 0.f);
            DCHECK(!glm::isinf(beta.y()));

            specular_bounce = ((int)flags & (int)BxDFType::BSDF_SPECULAR) != 0;
            if (((int)flags & (int)BxDFType::BSDF_SPECULAR) && (int)flags & (int)BxDFType::BSDF_TRANSMISSION)
            {
                float eta = isect._bsdf->_eta;

                eta_scale *= (glm::dot(wo, isect.n) > 0) ? (eta * eta) : 1 / (eta * eta);
            }
            ray = isect.SpawnRay(wi);

            Spectrum rrBeta = beta * eta_scale;
            if (rrBeta.maxComponentValue() < _rr_threshold && bounces > 3)
            {
                float q = glm::max(0.5f, 1 - rrBeta.maxComponentValue());
                if (sampler.Get1D() < q)
                    break;
                beta /= 1 - q;
                DCHECK(!glm::isinf(beta.y()));
            }
        }
        return L;
    }
}