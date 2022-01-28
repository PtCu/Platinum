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

#ifndef LIGHT_AREA_LIGHT_H_
#define LIGHT_AREA_LIGHT_H_

#include <core/light.h>
#include <math/transform.h>
#include <core/interaction.h>
#include <core/spectrum.h>
namespace platinum
{
    class DiffuseAreaLight : public AreaLight
    {
    public:
        //The surface it emits from is defined by a Shape
        DiffuseAreaLight(const Transform *light2world, const Spectrum &Le, int n_samples, Shape *shape, bool twoSided = false);
        virtual Spectrum L(const Interaction &inter, const glm::vec3 &w) const override
        {
            return (_two_sided || glm::dot(inter.n, w) > 0) ? _Lemit : Spectrum(0.f);
        }

    private:
        Spectrum _Lemit;
        Shape *_shape;
        float _area;
        bool _two_sided;
    };
}

#endif