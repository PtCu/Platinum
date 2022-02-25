

#include <accelerator/linear.h>

namespace platinum
{
    REGISTER_CLASS(LinearAggregate, "Linear");

    LinearAggregate::LinearAggregate(const PropertyTree &node)
        : Aggregate(node)
    {
    }

    LinearAggregate::LinearAggregate(const std::vector<Ptr<Primitive>> &hitables)
        : Aggregate(hitables)
    {
        for (const auto &hitable : _primitives)
        {
            _world_bounds = UnionBounds(_world_bounds, hitable->WorldBound());
        }
    }

    void LinearAggregate::Initialize()
    {
        for (const auto &hitable : _primitives)
        {
            _world_bounds = UnionBounds(_world_bounds, hitable->WorldBound());
        }
    }

    bool LinearAggregate::Hit(const Ray &ray) const
    {
        for (const auto &hitble : _primitives)
        {
            if (hitble->Hit(ray))
            {
                return true;
            }
        }
        return false;
    }

    bool LinearAggregate::Hit(const Ray &ray, SurfaceInteraction &inter) const
    {
        SurfaceInteraction tmp_inter;
        bool is_hit = false;
        for (const auto &hitble : _primitives)
        {
            if (hitble->Hit(ray, tmp_inter))
            {

                is_hit = true;
                inter = tmp_inter;
            }
        }
        return is_hit;
    }
}