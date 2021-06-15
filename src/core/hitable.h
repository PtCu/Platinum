#ifndef CORE_HITABLE_H_
#define CORE_HITABLE_H_

#include "platinum.h"
#include "rtti.h"
#include "shape.h"
#include "material.h"
#include "light.h"

#include <memory>

namespace platinum
{

    class Hitable : public Object
    {
    public:
        typedef std::shared_ptr<Hitable> ptr;

        virtual ~Hitable() = default;

        virtual bool hit(const Ray &ray) const = 0;
        virtual bool hit(const Ray &ray, SurfaceInteraction &iset) const = 0;

        virtual Bounds3f worldBound() const = 0;

        virtual const AreaLight *getAreaLight() const = 0;
        virtual const Material *getMaterial() const = 0;

        virtual void computeScatteringFunctions(SurfaceInteraction &isect, MemoryArena &arena,
                                                TransportMode mode, bool allowMultipleLobes) const = 0;

        virtual ClassType getClassType() const override { return ClassType::Hitable; }
    };

    class HitableObject final : public Hitable
    {
    public:
        typedef std::shared_ptr<HitableObject> ptr;

        HitableObject(const Shape::ptr &shape, const Material *material,
                      const AreaLight::ptr &areaLight);

        virtual bool hit(const Ray &ray) const override;
        virtual bool hit(const Ray &ray, SurfaceInteraction &iset) const override;

        virtual Bounds3f worldBound() const override;

        Shape *getShape() const;
        AreaLight::ptr getAreaLightPtr() const { return m_areaLight; }
        virtual const AreaLight *getAreaLight() const override;
        virtual const Material *getMaterial() const override;

        virtual void computeScatteringFunctions(SurfaceInteraction &isect, MemoryArena &arena,
                                                TransportMode mode, bool allowMultipleLobes) const override;

        virtual std::string toString() const override { return "HitableObject[]"; }

    private:
        Shape::ptr m_shape;
        AreaLight::ptr m_areaLight;

        const Material *m_material;
    };

    class HitableAggregate : public Hitable
    {
    public:
        virtual const AreaLight *getAreaLight() const override;
        virtual const Material *getMaterial() const override;

        virtual void computeScatteringFunctions(SurfaceInteraction &isect, MemoryArena &arena,
                                                TransportMode mode, bool allowMultipleLobes) const override;
    };
}

#endif