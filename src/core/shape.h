#ifndef CORE_SHAPE_H_
#define CORE_SHAPE_H_

#include "core/platinum.h"
#include "core/primitive.h"

namespace platinum
{
    enum ShapeType
    {
        EMesh,
        ESphere,
        EDisk,
        ECone,
        ECylinder
    };

    class Shape : public Primitive
    {
    public:
        Shape(const Transform *ObjectToWorld, const Transform *WorldToObject, bool reverseOrientation);

        virtual ~Shape();
        virtual Bounds3f ObjectBound() const = 0;
        virtual Bounds3f WorldBound() const = 0;
        virtual bool Intersect(const Ray &ray, float *tHit, SurfaceInteraction *isect, bool testAlphaTexture = true) const = 0;
        virtual bool IntersectP(const Ray &ray, bool testAlphaTexure = true) const
        {
            return Intersect(ray, nullptr, nullptr, testAlphaTexure);
        }
        virtual float Area() const = 0;
        const Transform *ObjectToWorld, *WorldToObject;
        const bool reverseOrientation;
    };

#endif