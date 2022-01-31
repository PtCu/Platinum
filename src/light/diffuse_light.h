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
        DiffuseAreaLight(const Transform &light2world, const Spectrum &Lemit, int n_samples, Shape *shape, bool two_sided = false)
            : AreaLight(light2world, n_samples), _Lemit(Lemit), _shape(shape),
              _two_sided(two_sided), _area(shape->Area()) {}

        virtual Spectrum L(const Interaction &inter, const glm::vec3 &w) const override
        {
            return (_two_sided || glm::dot(inter.n, w) > 0) ? _Lemit : Spectrum(0.f);
        }
        virtual Spectrum Power() const{
            return (_two_sided ? 2 : 1) * _Lemit * _area * Pi;
        }

        virtual Spectrum SampleLe(const glm::vec2 &u1, const glm::vec2 &u2, Ray &ray,
                                  glm::vec3 &nLight, float &pdfPos, float &pdfDir) const ;

        virtual void PdfLe(const Ray &, const glm::vec3 &, float &pdfPos, float &pdfDir) const ;


        virtual Spectrum SampleLi(const Interaction &inter, const glm::vec2 &u,
                                  glm::vec3 &wi, float &pdf, VisibilityTester &vis) const;
     
        virtual float PdfLi(const Interaction &inter, const glm::vec3 &wi) const;

    private:
        Spectrum _Lemit;
        Shape *_shape;
        float _area;
        bool _two_sided;
    };
}

#endif