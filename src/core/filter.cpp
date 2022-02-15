#include <core/filter.h>

namespace platinum
{


    Filter::Filter(const PropertyTree &root)
    {
        _radius = root.Get<Vector2f>("Radius");
        _inv_radius[0] = 1.f / _radius[0];
        _inv_radius[1] = 1.f / _radius[1];
       
    }
}