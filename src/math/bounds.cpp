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

#include <math/bounds.h>
#include <core/ray.h>
namespace platinum
{
    AABB::AABB()
    {
        float _min = std::numeric_limits<float>::lowest();
        float _max = std::numeric_limits<float>::max();
        _p_min = glm::vec3{_max, _max, _max};
        _p_max = glm::vec3{_min, _min, _min};
        is_valid_ = false;
    }
    AABB::AABB(const glm::vec3 &a, const glm::vec3 &b)
    {
        _p_min = glm::min(a, b);
        _p_max = glm::max(a, b);
        is_valid_ = true;
    }
    bool AABB::IsHit(const Ray &r) const
    {
        if (is_valid_ == false)
            return false;
        const glm::vec3 invDir = r.GetInvDirection();
        const AABB &bounds = *this;
        // Check for ray intersection against $x$ and $y$ slabs
        auto tMin = (bounds[r.IsDirNeg(0)].x - r.GetOrigin().x) * invDir.x;
        auto tMax = (bounds[1 - r.IsDirNeg(0)].x - r.GetOrigin().x) * invDir.x;
        auto tyMin = (bounds[r.IsDirNeg(1)].y - r.GetOrigin().y) * invDir.y;
        auto tyMax = (bounds[1 - r.IsDirNeg(1)].y - r.GetOrigin().y) * invDir.y;

        if (tMin > tyMax || tyMin > tMax)
            return false;
        if (tyMin > tMin)
            tMin = tyMin;
        if (tyMax < tMax)
            tMax = tyMax;

        // Check for ray intersection against $z$ slab
        auto tzMin = (bounds[r.IsDirNeg(2)].z - r.GetOrigin().z) * invDir.z;
        auto tzMax = (bounds[1 - r.IsDirNeg(2)].z - r.GetOrigin().z) * invDir.z;

        if (tMin > tzMax || tzMin > tMax)
            return false;
        if (tzMin > tMin)
            tMin = tzMin;
        if (tzMax < tMax)
            tMax = tzMax;

        return (tMin < r.GetMaxTime()) && (tMax > 0);
    }

    bool AABB::Hit(const Ray &ray, const glm::vec3 &inv_dir, const int dir_is_neg[3]) const
    {
        //取最大的t_min_x,y,z作为t_min
        //取最小的t_max_x,y,z作为t_max
        const AABB &bounds = *this;
        float t_min = (bounds[dir_is_neg[0]].x - ray._origin.x) * inv_dir.x;
        float t_max = (bounds[1 - dir_is_neg[0]].x - ray._origin.x) * inv_dir.x;
        float t_y_min = (bounds[dir_is_neg[1]].y - ray._origin.y) * inv_dir.y;
        float t_y_max = (bounds[1 - dir_is_neg[1]].y - ray._origin.y) * inv_dir.y;

        // 更新tmax，增加抗误差性
        t_max *= 1 + 2 * gamma(3);
        t_y_max *= 1 + 2 * gamma(3);
        if (t_min > t_y_max || t_y_min > t_max)
            return false;
        if (t_y_min > t_min)
            t_min = t_y_min;
        if (t_y_max < t_max)
            t_max = t_y_max;

        float t_z_min = (bounds[dir_is_neg[2]].z - ray._origin.z) * inv_dir.z;
        float t_z_max = (bounds[1 - dir_is_neg[2]].z - ray._origin.z) * inv_dir.z;

        t_z_max *= 1 + 2 * gamma(3);
        if (t_min > t_z_min || t_z_min > t_max)
            return false;
        if (t_z_min > t_min)
            t_min = t_z_min;
        if (t_z_max < t_max)
            t_max = t_z_max;
        return (t_min < ray._t_max) && (t_max > 0);
    }
    AABB AABB::Intersect(const AABB &b) const
    {
        return AABB(glm::max(_p_min, b._p_min), glm::min(_p_max, b._p_max));
    }
    void AABB::UnionWith(const AABB &aabb)
    {
        if (aabb.is_valid_)
        {
            if (is_valid_)
            {
                _p_min = glm::min(_p_min, aabb._p_min);
                _p_max = glm::max(_p_max, aabb._p_max);
            }
            else
            {
                _p_min = aabb.GetMin();
                _p_max = aabb.GetMax();
                is_valid_ = true;
            }
        }
    }
    void AABB::UnionWith(const glm::vec3 &p)
    {
        _p_min = glm::min(_p_min, p);
        _p_max = glm::max(_p_max, p);
        is_valid_ = true;
    }
    glm::vec3 AABB::Offset(const glm::vec3 &p) const
    {
        assert(_p_max.x > _p_min.x && _p_max.y > _p_min.y && _p_max.z > _p_min.z);
        glm::vec3 o = p - _p_min;
        o.x /= _p_max.x - _p_min.x;
        o.y /= _p_max.y - _p_min.y;
        o.z /= _p_max.z - _p_min.z;
        return o;
    }
    bool AABB::Overlaps(const AABB &p) const
    {
        bool x = (_p_max.x >= p._p_min.x) && (_p_min.x <= p._p_max.x);
        bool y = (_p_max.y >= p._p_min.y) && (_p_min.y <= p._p_max.y);
        bool z = (_p_max.z >= p._p_min.z) && (_p_min.z <= p._p_max.z);
        return (x && y && z);
    }
    bool AABB::Inside(const glm::vec3 &p) const
    {
        return p.x >= _p_min.x && p.x <= _p_max.x &&
               p.y >= _p_min.y && p.y <= _p_max.y &&
               p.z >= _p_min.z && p.z <= _p_max.z;
    }

    int AABB::MaxExtent() const
    {
        glm::vec3 d = Diagonal();
        if (d.x > d.y && d.x > d.z)
            return 0;
        else if (d.y > d.z)
            return 1;
        else
            return 2;
    }

}
