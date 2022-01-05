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

#include <core/bxdf.h>
namespace platinum {
    float FresnelDielectric::FrDielectric(float cosThetaI, float etaI, float etaT)const {
        cosThetaI = glm::clamp(cosThetaI, -1.f, 1.f);

        bool entering = cosThetaI > 0.f;
        // 如果如果入射角大于90° 
        // 则法线方向反了，cosThetaI取绝对值，对换两个折射率
        if (!entering) {
            std::swap(etaI, etaT);
            cosThetaI = std::abs(cosThetaI);
        }

        // 用斯涅耳定律计算sinThetaI
        float sinThetaI = glm::sqrt(glm::max(0.f, 1.f - cosThetaI * cosThetaI));
        float sinThetaT = etaI / etaT * sinThetaI;

        // 全内部反射情况
        if (sinThetaT >= 1) {
            return 1.f;
        }
        // 套公式
        float cosThetaT = glm::sqrt(glm::max(0.f, 1.f - sinThetaT * sinThetaT));
        float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT))
            / ((etaT * cosThetaI) + (etaI * cosThetaT));
        float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT))
            / ((etaI * cosThetaI) + (etaT * cosThetaT));
        return (Rparl * Rparl + Rperp * Rperp) / 2;
    }

    glm::vec3 FresnelConductor::FrConductor(float cosThetaI, const glm::vec3& etaI, const glm::vec3& etaT, const glm::vec3& kt)const {
        //套公式
        cosThetaI = glm::clamp(cosThetaI, -1.f, 1.f);
        glm::vec3 eta = etaT / etaI;
        glm::vec3 etak = kt / etaI;

        float cosThetaI2 = cosThetaI * cosThetaI;
        float sinThetaI2 = 1.f - cosThetaI2;
        glm::vec3 eta2 = eta * eta;
        glm::vec3 etak2 = etak * etak;

        glm::vec3 t0 = eta2 - etak2 - sinThetaI2;
        glm::vec3 a2plusb2 = glm::sqrt(t0 * t0 + 4.f * eta2 * etak2);
        glm::vec3 t1 = a2plusb2 + cosThetaI2;
        glm::vec3 a = glm::sqrt(0.5f * (a2plusb2 + t0));
        glm::vec3 t2 = (float)2 * cosThetaI * a;
        glm::vec3 Rs = (t1 - t2) / (t1 + t2);

        glm::vec3 t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
        glm::vec3 t4 = t2 * sinThetaI2;
        glm::vec3 Rp = Rs * (t3 - t4) / (t3 + t4);

        return 0.5f * (Rp + Rs);
    }
}