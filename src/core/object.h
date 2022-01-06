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

#ifndef CORE_OBJECT_H_
#define CORE_OBJECT_H_

#include <glm/glm.hpp>
#include <core/defines.h>
#include <core/material.h>
#include <core/shape.h>
#include <core/integrator.h>
#include <core/light.h>

namespace platinum
{
    class Object
    {
    public:
        Object() {}
        Object(std::shared_ptr<Material> mat = NULL) : _material(mat) {}
        virtual std::shared_ptr<Material> GetMaterial() const { return _material; }
        virtual ~Object() {}
        //TODO: 考虑将虚拟函数声明为非公用的，将公用函数声明为非虚拟的
        virtual HitRst Intersect(const Ray& r) = 0;
        virtual AABB GetBoundingBox() const = 0;
        virtual void Sample(HitRst& inter, float& pdf) const = 0;
        virtual float GetArea() const = 0;
    protected:
        std::shared_ptr<Material> _material;
    };


    class Hitable {
    public:
        typedef std::shared_ptr<Hitable> ptr;

        virtual ~Hitable() = default;

        virtual bool Hit(const Ray& ray) const = 0;
        virtual bool Hit(const Ray& ray, SurfaceInteraction& iset) const = 0;

        virtual AABB WorldBound() const = 0;

        virtual const AreaLight* GetAreaLight() const = 0;
        virtual const Material* GetMaterial() const = 0;

        virtual void ComputeScatteringFunctions(SurfaceInteraction& isect) const = 0;
    };

    class AHitableAggregate : public Hitable
    {
    public:

        virtual const AreaLight* GetAreaLight() const override;
        virtual const Material* GetMaterial() const override;

        virtual void ComputeScatteringFunctions(SurfaceInteraction& isect) const override;

    };



} // namespace platinum

#endif