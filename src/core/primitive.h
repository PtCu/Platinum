// The MIT License (MIT)

// Copyright (c) 2021 PtCu

//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

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

        virtual void ComputeScatteringFunctions(SurfaceInteraction &isect) const = 0;
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

        virtual void ComputeScatteringFunctions(SurfaceInteraction &isect) const override;

        virtual std::string ToString() const override { return "GeometricPrimitive"; }

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

        virtual void ComputeScatteringFunctions(SurfaceInteraction &isect) const override {}

    protected:
        std::vector<Ptr<Primitive>> _primitives;
        Bounds3f _world_bounds;
    };

} // namespace platinum

#endif