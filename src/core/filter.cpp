#include <core/filter.h>

namespace platinum
{
    Filter::Filter(const PropertyNode &root)
    {
        auto radius_iter = root.get_child("Radius").begin();
        _radius[0] = radius_iter->second.get_value<float>();
        ++radius_iter;
        _radius[1] = radius_iter->second.get_value<float>();
        _invRadius[0] = 1.f / _radius[0];
        _invRadius[1] = 1.f / _radius[1];
    }
}