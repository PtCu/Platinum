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

#ifndef CORE_BXDF_H_
#define CORE_BXDF_H_

#include <core/utilities.h>
#include <core/interaction.h>
#include <core/spectrum.h>
#include <core/memory.h>
namespace platinum
{
    enum class BxDFType
    {

        BSDF_REFLECTION = 1 << 0,
        BSDF_TRANSMISSION = 1 << 1,
        BSDF_DIFFUSE = 1 << 2,
        BSDF_GLOSSY = 1 << 3,
        BSDF_SPECULAR = 1 << 4,
        BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION,
    };
    class BxDF
    {
    public:
        BxDF(BxDFType type) : _type(type) {}

        virtual ~BxDF() = default;

        /**
         * @brief  type的组合可能多于_type，所以要看_type是否被包含在type中
         * @param  type            测试的type
         * @return true         _type包含在type中
         * @return false        _type没有包含在type中
         */
        bool MatchTypes(BxDFType type) const
        {
            return ((int)(_type) & (int)(type)) == (int)(_type);
        }

        /**
         * @brief    指定光的入射方向wi，求从wi散射到wo方向的光分布量
         *           wi方向一般是通过光采样得到的方向
         * @param  wo               出射方向
         * @param  wi               入射方向
         * @return Vector3f        radiance
         */
        virtual Spectrum F(const Vector3f &wo, const Vector3f &wi) const = 0;

        /**
         * @brief   返回入射方向为wi，出射方向为wo的概率密度(立体角空间)
         *          wi方向一般是通过光采样得到的方向
         * @param  wo               出射方向
         * @param  wi               入射方向
         * @return float    概率密度值
         */
        virtual float Pdf(const Vector3f &wo, const Vector3f &wi) const;

        /**
         * @brief   是F函数的简化，有时我们想仅仅输入出射方向，
         *          然后根据出射方向计算入射方向并返回相应的BxDF函数值。
         *          这在完美镜面反射和粗糙镜面反射中非常有用，因为此时它们的反射波瓣很窄，
         *          只占整个半球方向很小一部分，我们不想盲目暴力地遍历所有的入射方向，
         *          而是直接根据向量的反射特性获取入射方向（对于完美镜面反射，除了此方向其他方向上的贡献均为
         *          0，这时的BRDF是一个狄拉克函数），
         *          省去了很大部分的计算。
         *          It is crucial that any BxDF implementation that overrides the BxDF::Sample_f() method also override the BxDF::Pdf() method so that the two return consistent results.
        * @param  wo          出射方向
        * @param  wi          需要返回的入射方向
        * @param  sample      用于计算出射方向的样本点
        * @param  pdf         返回的对应方向上的概率密度函数
        * @param  sampledType BxDF的类型
         * @return Vector3f  radiance
         */
        virtual Spectrum SampleF(const Vector3f &wo, Vector3f &wi, const Vector2f &sample, float &pdf, BxDFType &sampleType) const;

        const BxDFType _type;
    };

    //   菲涅尔效应分2类材料讨论
    //   1.绝缘体，例如，水，空气，矿物油，玻璃等材质，通常这些材质折射率为1到3之间
    //   2.导体，电子可以在它们的原子晶格内自由移动，允许电流从一侧流向另一侧。
    //       当导体受到电磁辐射(如可见光)时，这种基本的原子特性会转化为一种截然不同的行为:
    //       导体会反射绝大部分的光，只有极小一部分会被物体吸收，并且仅仅在距离表面0.1微米处被吸收
    //       因此，只有非常非常薄的金属片，光才能穿透. 我们忽略这种现象，只计算反射分量
    //       与绝缘体不同的是，导体的折射率由复数表示 η' = η + i*k(k表示吸收系数)

    class Fresnel
    {
    public:
        virtual ~Fresnel() = default;
        /**
         * @brief
         * @param  cosI             入射角余弦值
         * @return Spectrum        返回对应入射角的菲涅尔函数值
         */
        virtual Spectrum Evaluate(float cosI) const = 0;
    };

    //菲涅尔项的Schlick近似
    class FresnelSchlick : public Fresnel
    {
    public:
        FresnelSchlick(const Spectrum F0) : _F0(F0) {}

        FresnelSchlick(float etaI, float etaT)
        {
            float F0_1 = (etaI - etaT) / (etaI + etaT);
            float F0 = F0_1 * F0_1;
            _F0 = Spectrum{F0};
        }
        virtual Spectrum Evaluate(float cosTheta) const override
        {
            auto pow5 = [](float i)
            { return (i * i) * (i * i) * i; };
            return _F0 + (Spectrum(1.f) - _F0) * pow5(1 - cosTheta);
        }

    private:
        //基础反射率
        Spectrum _F0;
    };

    //绝缘体的菲涅尔项
    class FresnelDielectric : public Fresnel
    {
    public:
        FresnelDielectric(float etaI, float etaT) : _etaI(etaI), _etaT(etaT) {}

        virtual Spectrum Evaluate(float cosThetaI) const override;

    private:
        float _etaI, _etaT;
    };

    class FresnelConductor : public Fresnel
    {
    public:
        FresnelConductor(const Spectrum &etaI, const Spectrum &etaT, const Spectrum &kt)
            : _etaI(etaI), _etaT(etaT), _kt(kt) {}

        virtual Spectrum Evaluate(float cosThetaI) const override;

    private:
        Spectrum _etaI, _etaT, _kt;
    };

    class FresnelNoOp : public Fresnel
    {
    public:
                virtual Spectrum Evaluate(float) const override { return Spectrum(1.); }
    };
}

#endif