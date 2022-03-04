

#ifndef CORE_SHAPE_H_
#define CORE_SHAPE_H_

#include <core/utilities.h>
#include <math/bounds.h>
#include <core/object.h>
namespace platinum
{
    class Shape : public Object
    {
    public:
        Shape(const PropertyTree &node) {}

        Shape(Transform *object2world, Transform *world2object)
            : _object2world(object2world), _world2object(world2object) {}

        virtual ~Shape() = default;

        void SetTransform(Transform *objectToWorld, Transform *worldToObject);

        virtual Bounds3f ObjectBound() const = 0;

        /**
         * @brief 将包围盒转化为世界坐标
         * @return AABB
         */
        virtual Bounds3f WorldBound() const;

        /**
         * @brief 判断是否相交并计算SurfaceInteration
         *
         * @param ray
         * @param tHit 相交时光线的时间t
         * @param inter 返回的SurfaceInteration
         * @return true
         * @return false
         */
        virtual bool Hit(const Ray &ray, float &tHit, SurfaceInteraction &inter) const = 0;

        /**
         * @brief 不计算SurfaceInteraction，仅用来判断是否相交
         *
         * @param ray
         * @return true
         * @return false
         */
        virtual bool Hit(const Ray &ray) const;

        virtual float Area() const = 0;

        // Sample a point on the surface of the shape and return the PDF with
        // respect to area on the surface.
        virtual Interaction Sample(const Vector2f &u, float &pdf) const = 0;

        virtual float Pdf(const Interaction &) const { return 1.f / Area(); }

        // Sample a point on the shape given a reference point |ref| and
        // return the PDF with respect to solid angle from |ref|.
        virtual Interaction Sample(const Interaction &ref, const Vector2f &u, float &pdf) const;

        virtual float Pdf(const Interaction &ref, const Vector3f &wi) const;

        // Returns the solid angle subtended by the shape w.r.t. the reference
        // point p, given in world space. Some shapes compute this value in
        // closed-form, while the default implementation uses Monte Carlo
        // integration; the nSamples parameter determines how many samples are
        // used in this case.
        virtual float SolidAngle(const Vector3f &p, int nSamples = 512) const;

    public:
        Transform *_object2world = nullptr, *_world2object = nullptr;
    };
}
#endif