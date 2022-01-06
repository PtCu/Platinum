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
        return true;
    }

    void Shape::SetTransform(Transform* object2world, Transform* world2object)
    {
        _object2world = object2world;
        _world2object = world2object;
    }

    Interaction Shape::Sample(const Interaction& ref, const glm::vec2& u, float& pdf) const
    {
        return Interaction{};
    }

    float Shape::Pdf(const Interaction& ref, const glm::vec3& wi) const
    {
        return 1.f;
    }

    float Shape::SolidAngle(const glm::vec3& p, int nSamples) const
    {

        return 1.0f;
    }
}