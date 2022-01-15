// Copyright 2022 ptcup
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

#include <core/shape.h>

namespace platinum {



    bool Shape::Hit(const Ray& ray) const
    {
        float t_hit = ray._t_max;
        SurfaceInteraction inter;
        return Hit(ray, t_hit, inter);
    }

    void Shape::SetTransform(Transform* object2world, Transform* world2object)
    {
        _object2world = object2world;
        _world2object = world2object;
    }

    AABB Shape::WorldBound()const {
        return _object2world->ExecOn(this->ObjectBound());
    }
    Interaction Shape::Sample(const Interaction& ref, const glm::vec2& u, float& pdf) const
    {
        Interaction inter = Sample(u, pdf);
        glm::vec3 wi = inter.p - ref.p;
        if (glm::dot(wi, wi) == 0)
        {
            pdf = 0;
        }
        else
        {
            wi = glm::normalize(wi);
            pdf *= glm::distance2(ref.p, inter.p) / glm::abs(glm::dot(inter.n, -wi));
            if (std::isinf(pdf))
                pdf = 0.f;
        }

        return inter;

    }

    float Shape::Pdf(const Interaction& ref, const glm::vec3& wi) const
    {
        Ray ray = ref.SpawnRay(wi);
        float t_hit;
        SurfaceInteraction inter_light;
        if (!Hit(ray, t_hit, inter_light))
            return 0.f;
        float pdf = glm::distance2(ref.p, inter_light.p) / (glm::abs(glm::dot(inter_light.n, -wi)) * Area());
        if (std::isinf(pdf))
            pdf = 0.f;
        return 1.f;
    }

    float Shape::SolidAngle(const glm::vec3& p, int nSamples) const
    {

        return 1.0f;
    }
}