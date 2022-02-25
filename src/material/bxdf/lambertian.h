

#ifndef MATERIAL_BXDF_LAMBERTIAN_H_
#define MATERIAL_BXDF_LAMBERTIAN_H_
#include <core/utilities.h>
#include <core/bxdf.h>

namespace platinum
{
    /**
    * @brief
    *    Lambertian反射，理想漫反射
    *    半球空间上各个方向的反射率相同
    *    在物理上其实并不可能，是一种理想模型
    **/
    class LambertianReflection : public BxDF
    {
    public:
        LambertianReflection(const Spectrum &R)
            : BxDF(BxDFType(static_cast<int>(BxDFType::BSDF_REFLECTION) | static_cast<int>(BxDFType::BSDF_DIFFUSE))),
              _R(R) {}

        virtual Spectrum F(const Vector3f &wo, const Vector3f &wi) const override
        {
            return _R * InvPi;
        }

    private:
        // 反照率（颜色方面）
        const Spectrum _R;
    };

}

#endif
