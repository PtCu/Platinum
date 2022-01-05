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

#ifndef MATERIAL_BXDF_LAMBERTIAN_H_
#define MATERIAL_BXDF_LAMBERTIAN_H_
#include <core/defines.h>
#include <core/bxdf.h>

namespace platinum {
    /**
    * @brief
    *    Lambertian反射，理想漫反射
    *    半球空间上各个方向的反射率相同
    *    在物理上其实并不可能，是一种理想模型
    **/
    class LambertianReflection :public BxDF {
    public:
        LambertianReflection(const glm::vec3& R)
            :BxDF(BxDFType(static_cast<int>(BxDFType::BSDF_REFLECTION) | static_cast<int>(BxDFType::BSDF_DIFFUSE))),
            _R(R) {}


        virtual glm::vec3 F(const glm::vec3& wo, const glm::vec3& wi)const override {
            return _R * InvPi;
        }
    private:
        // 反照率（颜色方面）
        const glm::vec3 _R;
    };

}


#endif
