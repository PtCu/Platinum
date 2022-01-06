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

namespace platinum {

    glm::vec3 WhittedIntegrator::Li(const Scene& scene, const Ray& ray, Sampler& sampler, int depth) {

        auto hit_rst = scene.RayIn(ray);
        glm::vec3 L;
        SurfaceInteraction inter;
        if (!hit_rst.is_hit) {
            //返回lights emission
            for (const auto& light : scene._lights)
                L += light->Le(ray);
            return L;
        }


        inter.ComputeScatteringFunctions(ray);
        // 没有bsdf
        if (!inter.bsdf)
        {
            return Li(scene, inter.SpawnRay(ray.GetDirection()), sampler, depth);
        }

        const glm::vec3& n(inter.n);
        glm::vec3 wo(inter.wo);
        // 如果光线打到光源，计算其发光值 -> Le (emission term)
        L += inter.Le(wo);

        //对每个光源，计算其贡献
        for (const auto& light : scene._lights) {
            float pdf;
            glm::vec3 wi;
            VisibilityTester visibility_tester;
            glm::vec3 sampled_li = light->SampleLi(inter, sampler.Get2D(), wi, pdf, visibility_tester);
            if (glm::vec3(0)==sampled_li|| pdf == 0)
                continue;
            glm::vec3 f = inter.bsdf->F(wo, wi);

            //如果所采样的光源上的光线没被遮挡
            if (visibility_tester.Unoccluded(scene)) {
                L += f * sampled_li * glm::dot(wi, n) / pdf;
            }



        }
        if (depth + 1 < _max_depth) {
            L += SpecularReflect(ray, inter, scene, sampler, depth);
            L += SpecularTransmit(ray, inter, scene, sampler, depth);
        }
        return L;
    }
}
