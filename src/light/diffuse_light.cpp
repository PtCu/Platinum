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

#include <light/diffuse_light.h>
#include <glm/gtx/norm.hpp>
namespace platinum
{

    Spectrum DiffuseAreaLight::SampleLe(const glm::vec2 &u1, const glm::vec2 &u2, Ray &ray,
                                        glm::vec3 &nLight, float &pdfPos, float &pdfDir) const
    {
        //TODO::
        return Spectrum(0.f);
    }

    void DiffuseAreaLight::PdfLe(const Ray &, const glm::vec3 &, float &pdfPos, float &pdfDir) const
    {
    }

    Spectrum DiffuseAreaLight::SampleLi(const Interaction &inter, const glm::vec2 &u,
                                        glm::vec3 &wi, float &pdf, VisibilityTester &vis) const
    {
        Interaction p_shape = _shape->Sample(inter, u, pdf);

        if (pdf == 0 || glm::length2(p_shape.p - inter.p) == 0)
        {
            pdf = 0;
            return 0.f;
        }
        wi = glm::normalize(p_shape.p - inter.p);
        vis = VisibilityTester(inter, p_shape);
        return L(p_shape, -wi);
    }

    float DiffuseAreaLight::PdfLi(const Interaction &inter, const glm::vec3 &wi) const
    {
        return _shape->Pdf(inter, wi);
    }
}