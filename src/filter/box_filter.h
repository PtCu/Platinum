#ifndef FILTER_BOX_FILTER_H_
#define FILTER_BOX_FILTER_H_

#include <core/filter.h>

namespace platinum
{
    class BoxFilter final : public Filter
    {
    public:
        BoxFilter(const glm::vec2 &radius) : Filter(radius) {}

        virtual float Evaluate(const glm::vec2 &p) const override
        {
            return 1.f;
        }
    };
}

#endif