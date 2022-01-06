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

#ifndef CORE_LIGHT_CPP_
#define CORE_LIGHT_CPP_
#include <glm/glm.hpp>
#include <core/ray.h>
#include <core/interaction.h>
#include <core/scene.h>

namespace platinum {
    //隐含类型指定为int
    enum class LightFlags : int
    {
        LightDeltaPosition = 1,
        LightDeltaDirection = 2,
        LightArea = 4,
        LightInfinite = 8
    };
    class Light {
    public:
        Light() = default;
        virtual ~Light() = default;;

        virtual glm::vec3 Le(const Ray& r)const { return glm::vec3(0); };

        virtual glm::vec3 SampleLe(const glm::vec2& u1, const glm::vec2& u2, Ray& ray,
            glm::vec3& nLight, float& pdfPos, float& pdfDir) const = 0;

        virtual void PdfLe(const Ray&, const glm::vec3&, float& pdfPos, float& pdfDir) const = 0;


        virtual glm::vec3 SampleLi(const Interaction& inter, const glm::vec2& u,
            glm::vec3& wi, float& pdf, VisibilityTester& vis)const;
        /**
         * @brief   返回在inter处采样光源时，对应的pdf函数值
         *          用于估计直接光照时，采样bsdf时生成的wi方向，对应的pdf函数值
         * @param  inter            My Param doc
         * @param  wi               My Param doc
         * @return float
         */
        virtual float PdfLi(const Interaction& inter, const glm::vec3& wi)const;
    };

    class VisibilityTester final {
    public:
        VisibilityTester() = default;
        VisibilityTester(const Interaction& p0, const Interaction& p1) :_p0(p0), _p1(p1) {

        }
        const Interaction& P0()const { return _p0; }
        const Interaction& P1()const { return _p1; }
        bool Unoccluded(const Scene& scene)const;
    private:
        Interaction _p0, _p1;
    };
}

#endif