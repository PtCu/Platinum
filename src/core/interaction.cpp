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

#include <core/interaction.h>

namespace platinum
{

    SurfaceInteraction::SurfaceInteraction(const glm::vec3 &p, const glm::vec2 &uv, const glm::vec3 &wo,
                                           const glm::vec3 &dpdu, const glm::vec3 &dpdv, const Shape *sh)
        : Interaction(p, glm::normalize(glm::cross(dpdu, dpdv)), wo), uv(uv), dpdu(dpdu), dpdv(dpdv), shape(sh)
    {
    }

    glm::vec3 SurfaceInteraction::Le(const glm::vec3 &w) const
    {
        const AreaLight *area = hitable->GetAreaLight();
        if (area)
            return area->L(*this, w);
        else
            return glm::vec3(0.f);
    }

    void SurfaceInteraction::ComputeScatteringFunctions(const Ray &ray)
    {
        hitable->ComputeScatteringFunctions(*this);
    }
}