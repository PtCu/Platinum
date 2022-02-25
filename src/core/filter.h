

#ifndef CORE_FILTER_H_
#define CORE_FILTER_H_

#include <core/utilities.h>
#include <core/object.h>

namespace platinum
{
    class Filter : public Object
    {
    public:
        Filter(const PropertyTree &root);

        Filter(const Vector2f &radius)
            : _radius(radius), _inv_radius(Vector2f(1 / radius.x, 1 / radius.y)) {}

        virtual ~Filter() = default;

        virtual float Evaluate(const Vector2f &p) const = 0;

        Vector2f _radius, _inv_radius;
    };
}

#endif