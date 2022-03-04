#ifndef LIGHT_INFINITE_LIGHT_H_
#define LIGHT_INFINITE_LIGHT_H_

#include <core/light.h>
#include <core/scene.h>

namespace platinum
{
    // class InfiniteAreaLight : public Light
    // {

    // public:
    //     InfiniteAreaLight(const Transform &light2world, const Spectrum &power, int nSamples, const std::string &texmap);

    //     void Proprocess(const Scene &scene)
    //     {
    //         scene.WorldBound().BoundingSphere(&_world_center, &_world_radius);
    //     }
    //     Spectrum Power() const;
    //     Spectrum Le(const Ray &ray) const;
    //     Spectrum SampleLi(const Interaction &ref, const Vector2f &u, Vector3f *wi,
    //                        float *pdf, VisibilityTester *vis) const;
    //     float PdfLi(const Interaction &, const Vector3f &) const;
    //     Spectrum SampleLe(const Vector2f &u1, const Vector2f &u2, float time,
    //                        Ray *ray,  Vector3f *nLight, float *pdfPos,
    //                        float *pdfDir) const;
    //     void PdfLe(const Ray &, const Vector3f &, float *pdfPos,
    //                 float *pdfDir) const;

    // private:
    //     Vector3f _world_center;
    //     float _world_radius;
    //     UPtr<Distribution2D> _distribution;
    // };
}
#endif