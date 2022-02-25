#ifndef FILTER_BOX_FILTER_H_
#define FILTER_BOX_FILTER_H_

#include <core/filter.h>

namespace platinum
{
    class BoxFilter final : public Filter
    {
    public:
        BoxFilter(const PropertyTree &node) : Filter(node) {}

        BoxFilter(const Vector2f &radius) : Filter(radius) {}

        virtual std::string ToString() const { return "BoxFilter"; }

        virtual float Evaluate(const Vector2f &p) const override
        {
            return 1.f;
        }
    };
}

#endif