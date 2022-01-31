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

#ifndef CORE_BSDF_CPP_
#define CORE_BSDF_CPP_

#include <core/utilities.h>
#include <core/interaction.h>
#include <core/bxdf.h>
namespace platinum
{

    class BSDF
    {
    public:
        BSDF(const SurfaceInteraction &s, float eta = 1)
            : _eta(eta), _ns(s.n), _ss(glm::normalize(s.dpdu)), _ts(glm::cross(_ns, _ss)) {}
        ~BSDF() = default;

        float Pdf(const glm::vec3 &wo, const glm::vec3 &wi, BxDFType type = BxDFType::BSDF_ALL) const;

        Spectrum F(const glm::vec3 &woW, const glm::vec3 &wiW, BxDFType flags = BxDFType::BSDF_ALL) const;

        Spectrum SampleF(const glm::vec3 &wo, glm::vec3 &wi, const glm::vec2 &u, float &pdf,
                         BxDFType &sampledType, BxDFType type = BxDFType::BSDF_ALL) const;

        void Add(std::shared_ptr<BxDF> b)
        {
            _BxDFs.push_back(b);
        }
        int NumComponents(BxDFType flags = BxDFType::BSDF_ALL) const;

        glm::vec3 World2Local(const glm::vec3 &v) const
        {
            //分别计算v在局部坐标基下的三个分量的投影
            return glm::vec3(glm::dot(v, _ss), glm::dot(v, _ts), glm::dot(v, _ns));
        }

        glm::vec3 Local2World(const glm::vec3 &v) const
        {
            return glm::vec3(
                _ss.x * v.x + _ts.x * v.y + _ns.x * v.z,
                _ss.y * v.x + _ts.y * v.y + _ns.y * v.z,
                _ss.z * v.x + _ts.z * v.y + _ns.z * v.z);
        }

        //Refractive index
        const float _eta;

    private:
        //局部坐标
        const glm::vec3 _ns, _ss, _ts;
        static constexpr int NumMaxBxDFs = 8;
        std::vector<std::shared_ptr<BxDF>> _BxDFs;
    };

}

#endif