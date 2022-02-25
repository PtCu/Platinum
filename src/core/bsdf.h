

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
            : _eta(eta), _ns(s.n), _ss(glm::normalize(s._dpdu)), _ts(glm::cross(_ns, _ss)) {}

        ~BSDF() = default;

        float Pdf(const Vector3f &wo, const Vector3f &wi, BxDFType type = BxDFType::BSDF_ALL) const;

        Spectrum F(const Vector3f &woW, const Vector3f &wiW, BxDFType flags = BxDFType::BSDF_ALL) const;

        Spectrum SampleF(const Vector3f &wo, Vector3f &wi, const Vector2f &u, float &pdf,
                         BxDFType &sampledType, BxDFType type = BxDFType::BSDF_ALL) const;

        void Add(BxDF *b)
        {
            CHECK_LT(_BxDF_num, _max_BxDF_num);
            _BxDFs[_BxDF_num++] = b;
        }
        int NumComponents(BxDFType flags = BxDFType::BSDF_ALL) const;

        Vector3f World2Local(const Vector3f &v) const
        {
            //分别计算v在局部坐标基下的三个分量的投影
            return Vector3f(glm::dot(v, _ss), glm::dot(v, _ts), glm::dot(v, _ns));
        }

        Vector3f Local2World(const Vector3f &v) const
        {
            return Vector3f(
                _ss.x * v.x + _ts.x * v.y + _ns.x * v.z,
                _ss.y * v.x + _ts.y * v.y + _ns.y * v.z,
                _ss.z * v.x + _ts.z * v.y + _ns.z * v.z);
        }

        //Refractive index
        const float _eta;

    private:
        //局部坐标
        const Vector3f _ns, _ss, _ts;
        static constexpr int _max_BxDF_num{8};
        int _BxDF_num{0};
        std::array<BxDF *, _max_BxDF_num> _BxDFs;
    };

}

#endif