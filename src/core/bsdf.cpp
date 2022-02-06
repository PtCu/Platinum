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

#include "bsdf.h"

namespace platinum
{

    int BSDF::NumComponents(BxDFType flags) const
    {
        int num = 0;
        for (const auto &b : _BxDFs)
        {
            if (b->MatchTypes(flags))
                ++num;
        }
        return num;
    }

    Spectrum BSDF::F(const Vector3f &woW, const Vector3f &wiW, BxDFType flags) const
    {
        Vector3f wi = World2Local(wiW), wo = World2Local(woW);
        if (0 == wo.z)
            return Spectrum(0.f);

        //BSDF的值为BRDF与BTDF之和
        //reflect表示入射光和出射光是否在一个半球内
        bool reflect = glm::dot(wiW, _ns) * glm::dot(woW, _ns) > 0;
        Spectrum f(0.f);
        for (const auto &b : _BxDFs)
        {
            if (b->MatchTypes(flags) &&
                ((reflect && (static_cast<int>(b->_type) & static_cast<int>(BxDFType::BSDF_REFLECTION))) ||
                 (!reflect && (static_cast<int>(b->_type) & static_cast<int>(BxDFType::BSDF_TRANSMISSION)))))
            {
                f += b->F(wo, wi);
            }
        }

        return f;
    }

    Spectrum BSDF::SampleF(const Vector3f &woWorld, Vector3f &wiWorld, const Vector2f &u, float &pdf,
                           BxDFType &sampledType, BxDFType type) const
    {
        // Choose which _BxDF_ to sample
        int matchingComps = NumComponents(type);
        if (0 == matchingComps)
        {
            pdf = 0;
            if (static_cast<int>(sampledType))
            {
                sampledType = BxDFType(0);
            }
            return Spectrum(0.f);
        }
        // 根据随机变量u随机选择bxdf组件
        int comp = glm::min((int)glm::floor(u[0] * matchingComps), matchingComps - 1);

        // Get _BxDF_ pointer for chosen component
        BxDF *bxdf = nullptr;
        int count = comp;
        for (const auto &b : _BxDFs)
        {
            if (b->MatchTypes(type) && count-- == 0)
            {
                bxdf = b;
                break;
            }
        }
        CHECK(bxdf != nullptr);
        // Remap _BxDF_ sample _u_ to $[0,1)^2$
        Vector2f uRemapped(glm::min(u[0] * matchingComps - comp, OneMinusEpsilon), u[1]);

        // Sample chosen _BxDF_
        Vector3f wi;
        Vector3f wo = World2Local(woWorld);
        if (wo.z == 0)
        {
            return Spectrum(0.f);
        }

        pdf = 0;
        if (static_cast<int>(sampledType))
        {
            sampledType = bxdf->_type;
        }

        // 对选中的bxdf采样
        Spectrum f = bxdf->SampleF(wo, wi, uRemapped, pdf, sampledType);

        if (0 == pdf)
        {
            if (static_cast<int>(sampledType))
            {
                sampledType = BxDFType(0);
            }
            return Spectrum(0.f);
        }

        wiWorld = Local2World(wi);

        // Compute overall PDF with all matching _BxDF_s
        if (!(static_cast<int>(bxdf->_type) & static_cast<int>(BxDFType::BSDF_SPECULAR)) && matchingComps > 1)
        {

            for (const auto &b : _BxDFs)
            {
                if (b != bxdf && b->MatchTypes(type))
                {
                    pdf += b->Pdf(wo, wi);
                }
            }
        }
        if (matchingComps > 1)
        {
            pdf /= matchingComps;
        }

        // Compute value of BSDF for sampled direction
        if (!(static_cast<int>(bxdf->_type) & static_cast<int>(BxDFType::BSDF_SPECULAR)))
        {
            bool reflect = glm::dot(wiWorld, _ns) * glm::dot(woWorld, _ns) > 0;
            f = Spectrum(0.f);
            for (const auto &b : _BxDFs)
            {
                if (b->MatchTypes(type) &&
                    ((reflect && (static_cast<int>(b->_type) & static_cast<int>(BxDFType::BSDF_REFLECTION))) ||
                     (!reflect && (static_cast<int>(b->_type) & static_cast<int>(BxDFType::BSDF_TRANSMISSION)))))
                {
                    f += b->F(wo, wi);
                }
            }
        }

        return f;
    }

    float BSDF::Pdf(const Vector3f &woWorld, const Vector3f &wiWorld, BxDFType flags) const
    {
        if (_BxDFs.empty())
        {
            return 0.f;
        }

        Vector3f wo = World2Local(woWorld), wi = World2Local(wiWorld);

        if (0 == wo.z)
        {
            return 0.;
        }

        float pdf = 0.f;
        int matchingComps = 0;
        for (const auto &b : _BxDFs)
        {
            if (b->MatchTypes(flags))
            {
                ++matchingComps;
                pdf += b->Pdf(wo, wi);
            }
        }
        float v = matchingComps > 0 ? pdf / matchingComps : 0.f;
        return v;
    }

}