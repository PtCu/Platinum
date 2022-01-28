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
#include <core/defines.h>
#include <core/ray.h>
#include <core/scene.h>
#include <math/transform.h>
#include <core/interaction.h>
#include <core/spectrum.h>

namespace platinum
{
    //隐含类型指定为int
    enum class LightFlags : int
    {
        LightDeltaPosition = 1,
        LightDeltaDirection = 2,
        LightArea = 4,
        LightInfinite = 8
    };
    class Light
    {
    public:
        Light() = default;
        virtual ~Light() = default;
        ;

        virtual glm::vec3 Le(const Ray &r) const { return glm::vec3(0); };

        virtual glm::vec3 SampleLe(const glm::vec2 &u1, const glm::vec2 &u2, Ray &ray,
                                   glm::vec3 &nLight, float &pdfPos, float &pdfDir) const = 0;

        virtual void PdfLe(const Ray &, const glm::vec3 &, float &pdfPos, float &pdfDir) const = 0;

        /**
         * @brief 返回从指定位置inter随机采样光源表面的点得到的辐射度
         * @param  inter            指定位置
         * @param  u                2维随机变量
         * @param  wi               返回光线入射方向
         * @param  pdf              返回对应随机变量的pdf值
         * @param  vis              可见测试器
         * @return glm::vec3        辐射度
         */
        virtual Spectrum SampleLi(const Interaction &inter, const glm::vec2 &u,
                                  glm::vec3 &wi, float &pdf, VisibilityTester &vis) const;
        /**
         * @brief   返回在inter处采样光源时，对应的pdf函数值
         *          用于估计直接光照时，采样bsdf时生成的wi方向，对应的pdf函数值
         * @param  inter            My Param doc
         * @param  wi               My Param doc
         * @return float
         */
        virtual float PdfLi(const Interaction &inter, const glm::vec3 &wi) const;

        //return their total emitted power
        virtual Spectrum Power() const = 0;

        //invoked prior to rendering
        // It includes the Scene as an argument so that the light source can determine
        //characteristics of the scene before rendering starts. The default implementation is empty,
        // but some implementations (e.g., DistantLight) use it to record a bound of the scene extent.
        virtual void Preprocess(const Scene &scene) {}

        int _flags;
        int _nSamples;
    };

    class AreaLight : public Light
    {
    public:
     
        AreaLight(const Transform &light2world, int n_samples);

        /**
         * @brief Given a point on the surface of the light represented by an Interaction and 
         *  should evaluate the area light’s emitted radiance, L, in the given outgoing direction.
         * 
         * @param inter 
         * @param w 
         * @return glm::vec3 
         */
        virtual Spectrum L(const Interaction &inter, const glm::vec3 &w) const = 0;
    };

}

#endif