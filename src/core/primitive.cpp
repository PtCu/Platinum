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

#include <core/primitive.h>

namespace platinum
{

    GeometricPrimitive::GeometricPrimitive(const Ptr<Shape> &shape, const Material *material,
                                           const Ptr<AreaLight> &area_light)
        : _shape(shape), _material(material), _area_light(area_light)
    {
        if (_area_light != nullptr)
        {
            _area_light->SetParent(this);
        }
    }

    bool GeometricPrimitive::Hit(const Ray &ray, SurfaceInteraction &inter) const
    {
        float t;
        if (!_shape->Hit(ray, t, inter))
            return false;
        ray._t_max = t;
        inter._hitable = this;
        return true;
    }

    void GeometricPrimitive::ComputeScatteringFunctions(SurfaceInteraction &inter, MemoryArena &arena) const
    {
        if (_material)
        {
            _material->ComputeScatteringFunctions(inter, arena);
        }
    }
}
