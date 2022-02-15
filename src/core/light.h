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

#ifndef CORE_LIGHT_H_
#define CORE_LIGHT_H_

#include <core/utilities.h>
#include <core/interaction.h>
#include <core/spectrum.h>
#include <math/transform.h>
#include <core/object.h>
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

    inline bool IsDeltaLight(int flags)
    {
        return flags & (int)LightFlags::LightDeltaPosition || flags & (int)LightFlags::LightDeltaDirection;
    }

    class Light: public Object
    {
    public:
        Light(const PropertyTree &node);

        Light(int flags, const Transform &light2world, int n_samples = 1)
            : _flags(flags), _num_samples(glm::max(1, n_samples)), _light2World(light2world), _world2Light(Inverse(light2world)){}

        virtual ~Light() = default;

        /**
         * @brief  环境光
         * 
         * @param r 
         * @return Spectrum 
         */
        virtual Spectrum Le(const Ray &r) const { return Spectrum(0.f); };

        /**
         * 根据Le分布采样光源
         * @param  u1     用于光源表面
         * @param  u2     用于生成方向
         * @param  time   时间
         * @param  ray    返回生成的ray
         * @param  nLight 光源表面法线
         * @param  pdfPos 位置PDF
         * @param  pdfDir 方向PDF
         * @return        [description]
         */
        virtual Spectrum SampleLe(const Vector2f &u1, const Vector2f &u2, Ray &ray,
                                  Vector3f &nLight, float &pdfPos, float &pdfDir) const = 0;

        virtual void PdfLe(const Ray &, const Vector3f &, float &pdfPos, float &pdfDir) const = 0;

        /**
         * @brief 返回从指定位置inter随机采样光源表面的点得到的辐射度
         * @param  inter            指定位置
         * @param  u                2维随机变量
         * @param  wi               返回光线入射方向
         * @param  pdf              返回对应随机变量的pdf值
         * @param  vis              可见测试器
         * @return Vector3f        辐射度
         */
        virtual Spectrum SampleLi(const Interaction &inter, const Vector2f &u,
                                  Vector3f &wi, float &pdf, VisibilityTester &vis) const = 0;
        /**
         * @brief   返回在inter处采样光源时，对应的pdf函数值
         *          用于估计直接光照时，采样bsdf时生成的wi方向，对应的pdf函数值
         * @param  inter            My Param doc
         * @param  wi               My Param doc
         * @return float
         */
        virtual float PdfLi(const Interaction &inter, const Vector3f &wi) const = 0;

        /**
         * @brief Return their total emitted power
         * 
         * @return Spectrum 
         */
        virtual Spectrum Power() const = 0;


        /**
         * @brief   Invoked prior to rendering.
         *          It includes the Scene as an argument so that the light source can determine
         *          characteristics of the scene before rendering starts. The default implementation is empty,
         *          but some implementations (e.g., DistantLight) use it to record a bound of the scene extent.
         * 
         * @param scene 
         */
        virtual void Preprocess(const Scene &scene) {}

        int _flags;
        int _num_samples;

    protected:
        Transform _light2World, _world2Light;
    };

    class AreaLight : public Light
    {
    public:
        AreaLight(const PropertyTree &node);
        
        AreaLight(const Transform &light2world, int n_samples)
            : Light(static_cast<int>(LightFlags::LightArea), light2world, n_samples) {}

        /**
         * @brief Given a point on the surface of the light represented by an Interaction and 
         *  should evaluate the area light’s emitted radiance, L, in the given outgoing direction.
         * 
         * @param inter 
         * @param w 
         * @return Vector3f 
         */
        virtual Spectrum L(const Interaction &inter, const Vector3f &w) const = 0;
    };

    class VisibilityTester final
    {
    public:
        VisibilityTester() = default;

        VisibilityTester(const Interaction &p0, const Interaction &p1)
            : _p0(p0), _p1(p1) {}

        const Interaction &P0() const { return _p0; }

        const Interaction &P1() const { return _p1; }

        bool Unoccluded(const Scene &scene) const;

    private:
        Interaction _p0, _p1;
    };
}

#endif