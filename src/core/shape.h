// Copyright 2021 ptcup
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CORE_SHAPE_H_
#define CORE_SHAPE_H_

#include <core/utilities.h>
#include <math/bounds.h>
namespace platinum
{
    class Shape
    {
    public:
        Shape(Transform *object2world, Transform *world2object)
            : _object2world(object2world), _world2object(world2object) {}
        virtual ~Shape() = default;
        void SetTransform(Transform *objectToWorld, Transform *worldToObject);

        virtual Bounds3f ObjectBound() const = 0;

        /**
         * @brief 将包围盒转化为世界坐标
         * @return AABB 
         */
        virtual Bounds3f WorldBound() const;

        /**
         * @brief 判断是否相交并计算SurfaceInteration
         * 
         * @param ray 
         * @param tHit 相交时光线的时间t
         * @param inter 返回的SurfaceInteration
         * @return true 
         * @return false 
         */
        virtual bool Hit(const Ray &ray, float &tHit, SurfaceInteraction &inter) const = 0;

        /**
         * @brief 不计算SurfaceInteraction，仅用来判断是否相交
         * 
         * @param ray 
         * @return true 
         * @return false 
         */
        virtual bool Hit(const Ray &ray) const;

        virtual float Area() const = 0;

        // Sample a point on the surface of the shape and return the PDF with
        // respect to area on the surface.
        virtual Interaction Sample(const Vector2f &u, float &pdf) const = 0;
        virtual float Pdf(const Interaction &) const { return 1 / Area(); }

        // Sample a point on the shape given a reference point |ref| and
        // return the PDF with respect to solid angle from |ref|.
        virtual Interaction Sample(const Interaction &ref, const Vector2f &u, float &pdf) const;
        virtual float Pdf(const Interaction &ref, const Vector3f &wi) const;

        // Returns the solid angle subtended by the shape w.r.t. the reference
        // point p, given in world space. Some shapes compute this value in
        // closed-form, while the default implementation uses Monte Carlo
        // integration; the nSamples parameter determines how many samples are
        // used in this case.
        virtual float SolidAngle(const Vector3f &p, int nSamples = 512) const;

        Transform *_object2world = nullptr, *_world2object = nullptr;
    };
}
#endif