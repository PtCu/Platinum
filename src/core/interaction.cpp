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
#include <core/spectrum.h>
#include <core/primitive.h>
namespace platinum
{

    SurfaceInteraction::SurfaceInteraction(const Vector3f &p, const Vector2f &uv, const Vector3f &wo,
                                           const Vector3f &dpdu, const Vector3f &dpdv, const Shape *sh)
        : Interaction(p, glm::normalize(glm::cross(dpdu, dpdv)), wo), _uv(uv), _dpdu(dpdu), _dpdv(dpdv), _shape(sh)
    {
    }

    Spectrum SurfaceInteraction::Le(const Vector3f &w) const
    {
        const AreaLight *area = _hitable->GetAreaLight();
        if (area)
            return area->L(*this, w);
        else
            return Spectrum(0.f);
    }

    void SurfaceInteraction::ComputeScatteringFunctions(const Ray &ray,MemoryArena &arena)
    {
        _hitable->ComputeScatteringFunctions(*this,arena);
    }
}