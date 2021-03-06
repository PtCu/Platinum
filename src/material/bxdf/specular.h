
#ifndef MATERIAL_BXDF_SPECULAR_H_
#define MATERIAL_BXDF_SPECULAR_H_

#include <core/utilities.h>
#include <core/bxdf.h>

namespace platinum
{
    /**
    * @brief
    *  理想镜面反射    
    * 
    */
    class SpecularReflection : public BxDF
    {
    public:
        SpecularReflection(const Spectrum &R, Fresnel *fresnel)
            : BxDF(BxDFType(static_cast<int>(BxDFType::BSDF_REFLECTION) | static_cast<int>(BxDFType::BSDF_SPECULAR))),
              _R(R),
              _fresnel(fresnel) {}

        //完美的镜面反射和透射非常特殊，给定一个入射方向ωi，则该表面散射的方向有且仅有一个ωo，
        //而非遍布整个半球方向。所以用用于处理特殊BxDF的SampleF函数来计算完美镜面反射的BRDF。
        virtual Spectrum F(const Vector3f &wo, const Vector3f &wi) const override
        {
            return Spectrum(0.f);
        }

        //给定入射方向ωi，我们可以很容易地求出相应的完美镜面反射方向ωo，
        virtual Spectrum SampleF(const Vector3f &wo, Vector3f &wi, const Vector2f &sample, float &pdf, BxDFType &sampleType) const override;
        virtual float Pdf(const Vector3f &wo, const Vector3f &wi) const override { return 0.f; }

    private:
        //反照率(颜色方面)
        const Spectrum _R;
        //菲涅尔项
        const Fresnel *_fresnel;
    };

}

#endif
