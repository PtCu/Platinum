#ifndef CORE_PRIMITIVE_H_
#define CORE_PRIMITIVE_H_

#include "core/platinum.h"
#include "math/bound.h"

namespace Platinum
{
    class Object
    {
    public:
        virtual nloJson toJson() const
        {
            return nloJson();
        }
        virtual virtual ~Object() {}
    };

    class Primitive : public Object
    {
        virtual Bounds3f WorldBound() const = 0;
        virtual bool Intersect(const Ray &r, SurfaceInteraction *) const = 0;
        virtual bool IntersectP(const Ray &r) const = 0;
        virtual void ComputeScatteringFunctions() const = 0;
    };

    class GeometricPrimitive : public Primitive
    {
    public:
        virtual Bounds3f WorldBound() const;
        virtual bool Intersect(const Ray &r, SurfaceInteraction *isect) const;
        virtual bool IntersectP(const Ray &r) const;
        GeometricPrimitive(const std::shared_ptr<Shape> &shape);
        void ComputeScatteringFunctions() const;

    private:
        std::shared_ptr<Shape> shape;
    };


} // namespace Platinum

#endif