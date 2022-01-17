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

#ifndef CORE_INTERSECTION_H_
#define CORE_INTERSECTION_H_

#include <glm/glm.hpp>
#include <core/defines.h>
#include <core/material.h>
#include <core/ray.h>
#include <core/shape.h>
#include <core/primitive.h>
namespace platinum
{

    //SurfaceInteraction和MediumInteraction的基类
    class Interaction
    {
    public:
        Interaction() = default;
        explicit Interaction(const glm::vec3& p) : p(p) {}
        Interaction(const glm::vec3& p, const glm::vec3& wo) : p(p), wo(normalize(wo)) {}
        Interaction(const glm::vec3& p, const glm::vec3& n, const glm::vec3& wo)
            : p(p), wo(glm::normalize(wo)), n(n) {}

        inline Ray SpawnRay(const glm::vec3& d) const
        {
            glm::vec3 o = p;
            return Ray(o, d, std::numeric_limits<float>::max());
        }

        inline Ray SpawnRayTo(const glm::vec3& p2) const
        {
            glm::vec3 origin = p;
            return Ray(origin, p2 - p, 1.f - ShadowEpsilon);
        }

        inline Ray SpawnRayTo(const Interaction& it) const
        {
            glm::vec3 origin = p;
            glm::vec3 target = it.p;
            glm::vec3 d = target - origin;
            return Ray(origin, d, 1.f - ShadowEpsilon);
        }

        glm::vec3 p;  //surface point
        glm::vec3 wo; //outgoing direction
        glm::vec3 n;  //normal vector
    };

    /**
     * @brief Having this abstraction lets most of the system work with points on surfaces
     * without needing to consider the particular type of geometric shape the points lie on
     */
    class SurfaceInteraction final : public Interaction
    {
    public:
        SurfaceInteraction() = default;
        SurfaceInteraction(const glm::vec3& p, const glm::vec2& uv, const glm::vec3& wo,
            const glm::vec3& dpdu, const glm::vec3& dpdv, const Shape* sh);

        glm::vec3 Le(const glm::vec3& w) const;

        void ComputeScatteringFunctions(const Ray& ray);

    public:
        std::shared_ptr<BSDF> bsdf{ nullptr };
        const Shape* shape{ nullptr };
        const Primitive* hitable{ nullptr };


        /**
         * @brief   (u,v)是p点参数化后的表面坐标（如纹理坐标）
         *          dpdu和dpdv是p在u,v方向的微分。二者不必正交
         *
         */
        glm::vec2 uv;
        glm::vec3 dpdu, dpdv;
    };

    class VisibilityTester final
    {
    public:
        VisibilityTester() = default;
        VisibilityTester(const Interaction& p0, const Interaction& p1) : _p0(p0), _p1(p1)
        {
        }
        const Interaction& P0() const { return _p0; }
        const Interaction& P1() const { return _p1; }
        bool Unoccluded(const Scene& scene) const;

    private:
        Interaction _p0, _p1;
    };
}
#endif
