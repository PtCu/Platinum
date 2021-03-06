

#ifndef GEOMETRY_SPHERE_H_
#define GEOMETRY_SPHERE_H_

#include <core/primitive.h>
#include <math/ray.h>
#include <core/interaction.h>
#include <math/bounds.h>
#include <math/rand.h>
#include <core/shape.h>

namespace platinum
{ /**
     * @brief
     *      所有球体对象的object坐标系都是以球心为原点
     *      以z坐标的范围跟φ来裁剪球体，
     *      球的参数方程可以表示为(和高数的球坐标系的Φ、θ相反)
     *      x = rsinθcosφ
     *      y = rsinθsinφ
     *      z = rcosθ
     *      其中
     *      -r ≤ z ≤ r
     *      0 < φ ≤ 2π ，φ为转动的角度
     *      表面参数uv为
     *      φ = u * φmax
     *      θ = θmin + v * (θmax - θmin)
     *
     */
    class Sphere : public Shape
    {
    public:
        Sphere(const PropertyTree &node);

        Sphere(Transform *object2world, Transform *world2object, const float radius)
            : Shape(object2world, world2object), _radius(radius) {}

        virtual ~Sphere() = default;

        virtual float Area() const override;

        virtual Interaction Sample(const Vector2f &u, float &pdf) const override;

        virtual Interaction Sample(const Interaction &ref, const Vector2f &u, float &pdf) const override;

        virtual float Pdf(const Interaction &ref, const Vector3f &wi) const override;

        virtual Bounds3f ObjectBound() const override;

        virtual bool Hit(const Ray &ray) const override;

        virtual bool Hit(const Ray &ray, float &t_hit, SurfaceInteraction &inter) const override;

        virtual float SolidAngle(const Vector3f &p, int nSamples = 512) const override;

        virtual std::string ToString() const { return "Sphere"; }

    private:
        float _radius;
    };

} // namespace platinum

#endif
