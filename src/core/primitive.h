

#ifndef CORE_PRIMITIVE_H_
#define CORE_PRIMITIVE_H_

#include <core/utilities.h>
#include <core/material.h>
#include <core/shape.h>
#include <core/light.h>
#include <math/bounds.h>

namespace platinum
{

    class Primitive : public Object
    {
    public:
        typedef Ptr<Primitive> ptr;

        virtual ~Primitive() = default;

        virtual bool Hit(const Ray &ray) const = 0;

        virtual bool Hit(const Ray &ray, SurfaceInteraction &iset) const = 0;

        virtual Bounds3f WorldBound() const = 0;

        virtual const AreaLight *GetAreaLight() const = 0;

        virtual const Material *GetMaterial() const = 0;

        virtual void ComputeScatteringFunctions(SurfaceInteraction &isect, MemoryArena &arena) const = 0;
    };

    class GeometricPrimitive : public Primitive
    {
    public:
        GeometricPrimitive(const Ptr<Shape> &shape, const Material *material,
                           const Ptr<AreaLight> &area_light);

        virtual bool Hit(const Ray &ray) const override { return _shape->Hit(ray); }

        virtual bool Hit(const Ray &ray, SurfaceInteraction &iset) const override;

        virtual Bounds3f WorldBound() const override { return _shape->WorldBound(); }

        Shape *GetShape() const { return _shape.get(); }

        Ptr<AreaLight> GetAreaLightPtr() const { return _area_light; }

        virtual const AreaLight *GetAreaLight() const override { return _area_light.get(); }

        virtual const Material *GetMaterial() const override { return _material; }

        virtual void ComputeScatteringFunctions(SurfaceInteraction &isect, MemoryArena &arena) const override;

        virtual virtual std::string ToString() const override { return "GeometricPrimitive"; }

    private:
        Ptr<Shape> _shape;
        Ptr<AreaLight> _area_light;

        const Material *_material;
    };

    class Aggregate : public Primitive
    {
    public:
        Aggregate(const PropertyTree &node) {}

        Aggregate() = default;

        virtual void Initialize() {}

        Aggregate(const std::vector<Ptr<Primitive>> &primitives)
            : _primitives(primitives) {}
        /**
         * @brief Set the Primitives object in a move constructor way.
         * 
         * @param primitives 
         */
        void SetPrimitives(const std::vector<Ptr<Primitive>> &primitives) { _primitives = std::move(primitives); }

        virtual const AreaLight *GetAreaLight() const override { return nullptr; }

        virtual const Material *GetMaterial() const override { return nullptr; }

        virtual void ComputeScatteringFunctions(SurfaceInteraction &isect, MemoryArena &arena) const override {}

    protected:
        std::vector<Ptr<Primitive>> _primitives;
        Bounds3f _world_bounds;
    };

} // namespace platinum

#endif