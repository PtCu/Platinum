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
#include <core/primitive.h>
namespace platinum
{
    REGISTER_CLASS(DiffuseAreaLight, "DiffuseAreaLight");

    DiffuseAreaLight::DiffuseAreaLight(const PropertyTree &node)
        : AreaLight(node), _shape(nullptr)
    {
        // auto spectrum_node = node.get_child("Radiance");
        // auto iter = spectrum_node.begin();
        // std::array<float, 3> spectrum;
        // for (size_t i = 0; i < 3; ++i, ++iter)
        // {
        //     spectrum[i] = iter->second.get_value<float>();
        // }

        Vector3f spectrum = node.Get<Vector3f>("Radiance");
        _Lemit = Spectrum::fromRGB(spectrum);
        _two_sided = node.Get<bool>("TwoSided");
    }
    void DiffuseAreaLight::SetParent(Object *parent)
    {
       
        auto p_node = dynamic_cast<GeometricPrimitive *>(parent);
        if (!p_node)
        {
            LOG(ERROR) << "DiffuseAreaLight's parent can only be primitive!";
        }
        _shape =p_node->GetShape();
        _area = _shape->Area();
        _light2World = *_shape->_object2world;
        _world2Light = *_shape->_world2object;

       
    }

    Spectrum DiffuseAreaLight::SampleLe(const Vector2f &u1, const Vector2f &u2, Ray &ray,
                                        Vector3f &nLight, float &pdfPos, float &pdfDir) const
    {
        //TODO::
        return Spectrum(0.f);
    }

    void DiffuseAreaLight::PdfLe(const Ray &, const Vector3f &, float &pdfPos, float &pdfDir) const
    {
    }

    Spectrum DiffuseAreaLight::SampleLi(const Interaction &inter, const Vector2f &u,
                                        Vector3f &wi, float &pdf, VisibilityTester &vis) const
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

    float DiffuseAreaLight::PdfLi(const Interaction &inter, const Vector3f &wi) const
    {
        return _shape->Pdf(inter, wi);
    }
}