

#ifndef LIGHT_AREA_LIGHT_H_
#define LIGHT_AREA_LIGHT_H_

#include <core/light.h>
#include <core/shape.h>

namespace platinum
{
    class DiffuseAreaLight final : public AreaLight
    {
    public:
        DiffuseAreaLight(const PropertyTree &node);

        //The surface it emits from is defined by a Shape
        DiffuseAreaLight(const Transform &light2world, const Spectrum &Lemit, int n_samples, Shape *shape, bool two_sided = false)
            : AreaLight(light2world, n_samples), _Lemit(Lemit), _shape(shape),
              _two_sided(two_sided), _area(shape->Area())
        {
        }

        virtual Spectrum L(const Interaction &inter, const Vector3f &w) const override
        {
            return (_two_sided || glm::dot(inter.n, w) > 0) ? _Lemit : Spectrum(0.f);
        }
        virtual Spectrum Power() const
        {
            return (_two_sided ? 2 : 1) * _Lemit * _area * Pi;
        }

        virtual Spectrum SampleLe(const Vector2f &u1, const Vector2f &u2, Ray &ray,
                                  Vector3f &nLight, float &pdfPos, float &pdfDir) const;

        virtual void PdfLe(const Ray &, const Vector3f &, float &pdfPos, float &pdfDir) const;

        virtual Spectrum SampleLi(const Interaction &inter, const Vector2f &u,
                                  Vector3f &wi, float &pdf, VisibilityTester &vis) const;

        virtual float PdfLi(const Interaction &inter, const Vector3f &wi) const;

        virtual std::string ToString() const { return "DiffuseAreaLight"; }

        virtual void DiffuseAreaLight::SetParent(Object *parent) override;

    private:
        Spectrum _Lemit;
        Shape *_shape;
        float _area;
        bool _two_sided;
    };
}

#endif