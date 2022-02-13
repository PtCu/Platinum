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

#include "whitted_integrator.h"

namespace platinum
{

    REGISTER_CLASS(WhittedIntegrator, "Whitted");

    WhittedIntegrator::WhittedIntegrator(const PropertyNode &root)
        : SamplerIntegrator(nullptr, nullptr), _max_depth(root.get<int>("Depth"))
    {
        _sampler = UPtr<Sampler>(static_cast<Sampler *>(ObjectFactory::CreateInstance(root.get<std::string>("Sampler.Type"), root.get_child("Sampler"))));

        _camera = UPtr<Camera>(static_cast<Camera *>(ObjectFactory::CreateInstance(root.get<std::string>("Camera.Type"), root.get_child("Camera"))));

        
     }

    //https://pbr-book.org/3ed-2018/Introduction/pbrt_System_Overview#WhittedIntegrator
    Spectrum WhittedIntegrator::Li(const Scene &scene, const Ray &ray, Sampler &sampler, int depth) const
    {

        Spectrum L{0.f};

        SurfaceInteraction inter;
        // Find closest ray intersection or return background radiance
        if (!scene.Hit(ray, inter))
        {
            //返回lights emission
            for (const auto &light : scene._lights)
                L += light->Le(ray);
            return L;
        }

        const Vector3f &n = inter.n;
        Vector3f wo = inter.wo;

        inter.ComputeScatteringFunctions(ray);
        // 没有bsdf
        if (!inter._bsdf)
        {
            return Li(scene, inter.SpawnRay(ray.GetDirection()), sampler, depth);
        }

        // 如果光线打到光源，计算其发光值 -> Le (emission term)
        L += inter.Le(wo);

        //对每个光源，计算其贡献
        for (const auto &light : scene._lights)
        {
            float pdf;
            Vector3f wi;
            VisibilityTester visibility_tester;
            Spectrum sampled_li = light->SampleLi(inter, sampler.Get2D(), wi, pdf, visibility_tester);

            if (sampled_li.isBlack() || pdf == 0)
                continue;

            Spectrum f = inter._bsdf->F(wo, wi);

            //如果所采样的光源上的光线没被遮挡
            if (!f.isBlack() && visibility_tester.Unoccluded(scene))
            {
                L += f * sampled_li * glm::abs(glm::dot(wi, n)) / pdf;
            }
        }
        if (depth + 1 < _max_depth)
        {
            // Trace rays for specular reflection and refraction
            L += SpecularReflect(ray, inter, scene, sampler, depth);
            L += SpecularTransmit(ray, inter, scene, sampler, depth);
        }
        return L;
    }
}
