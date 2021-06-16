#ifndef ACCELERATORS_LINEAR_AGGREGATE_H_
#define ACCELERATORS_LINEAR_AGGREGATE_H_

#include "hitable.h"

namespace platinum
{
    class LinearAggregate final : public HitableAggregate
    {
    public:
        typedef std::shared_ptr<LinearAggregate> ptr;

        LinearAggregate(const std::vector<Hitable::ptr> &hitables);
        virtual bool hit(const Ray &ray) const override;
        virtual bool hit(const Ray &ray, SurfaceInteraction &iset) const override;

        virtual Bounds3f worldBound() const override;

        virtual std::string toString() const override { return "LinearAggregate[]"; }

    private:
        using HitableBuffer = std::vector<Hitable::ptr>;
        HitableBuffer m_hitableList;
        Bounds3f m_worldBounds;
    };
} // namespace platinum

#endif