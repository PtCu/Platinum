#ifndef CORE_FILTER_H_
#define CORE_FILTER_H_

#include "platinum.h"
#include "rtti.h"
#include "math_utils.h"

namespace platinum
{

    class Filter : public Object
    {
    public:
        virtual ~Filter() = default;

        Filter(const PropertyList &props);
        Filter(const Vector2f &radius)
            : m_radius(radius), m_invRadius(Vector2f(1 / radius.x, 1 / radius.y)) {}

        virtual Float evaluate(const Vector2f &p) const = 0;

        virtual ClassType getClassType() const override { return ClassType::FilterType; }

        const Vector2f m_radius, m_invRadius;
    };

}

#endif