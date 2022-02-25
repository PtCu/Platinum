
#ifndef ACCELERATOR_LINEAR_AGGREGATE_CPP_
#define ACCELERATOR_LINEAR_AGGREGATE_CPP_

#include <core/primitive.h>

namespace platinum
{
    class LinearAggregate final : public Aggregate
    {
    public:
        LinearAggregate(const PropertyTree &node);

        LinearAggregate() = default;

        LinearAggregate(const std::vector<Ptr<Primitive>> &);

        virtual void Initialize();

        virtual bool Hit(const Ray &ray) const override;

        virtual bool Hit(const Ray &ray, SurfaceInteraction &inter) const;

        virtual Bounds3f WorldBound() const override { return _world_bounds; }

        virtual std::string ToString() const { return "LinearAggregate"; }
    };
}

#endif
