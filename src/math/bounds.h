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

#ifndef CORE_AABB_H_
#define CORE_AABB_H_

#include <glm/glm.hpp>
#include <core/defines.h>
namespace platinum
{
    template<typename T>
    class Bounds3
    {
    public:
        Bounds3()
        {
            T minNum = std::numeric_limits<T>::lowest();
            T maxNum = std::numeric_limits<T>::max();
            _p_min = glm::vec<3, T>(maxNum, maxNum, maxNum);
            _p_max = glm::vec<3, T>(minNum, minNum, minNum);
        }

        explicit Bounds3(const glm::vec<3, T>& p) : _p_min(p), _p_max(p) {}

        Bounds3(const glm::vec<3, T>& p1, const glm::vec<3, T>& p2) :
            _p_min(glm::min(p1.x, p2.x), glm::min(p1.y, p2.y), glm::min(p1.z, p2.z)),
            _p_max(glm::max(p1.x, p2.x), glm::max(p1.y, p2.y), glm::max(p1.z, p2.z)) {}

        glm::vec<3, T>& operator[](int i) { return (i == 0) ? _p_min : _p_max; }
        const glm::vec<3, T>& operator[](int i) const { return (i == 0) ? _p_min : _p_max; }

        bool operator==(const Bounds3<T>& b) const { return b._p_min == _p_min && b._p_max == _p_max; }
        bool operator!=(const Bounds3<T>& b) const { return b._p_min != _p_min || b._p_max != _p_max; }

        glm::vec<3, T> Corner(int cor) const
        {
            DCHECK(cor >= 0 && cor < 8);
            return glm::vec<3, T>((*this)[(cor & 1)].x, (*this)[(cor & 2) ? 1 : 0].y, (*this)[(cor & 4) ? 1 : 0].z);
        }

        glm::vec<3, T> Diagonal() const { return _p_max - _p_min; }

        T SurfaceArea() const
        {
            glm::vec<3, T> d = Diagonal();
            return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
        }

        T Volume() const
        {
            glm::vec<3, T> d = Diagonal();
            return d.x * d.y * d.z;
        }

        int MaximumExtent() const
        {
            glm::vec<3, T> d = Diagonal();
            if (d.x > d.y && d.x > d.z)
                return 0;
            else if (d.y > d.z)
                return 1;
            else
                return 2;
        }

        glm::vec<3, T> Lerp(const glm::vec3& t) const
        {
            return glm::vec<3, T>(
                plainum::lerp(t.x, _p_min.x, _p_max.x),
                plainum::lerp(t.y, _p_min.y, _p_max.y),
                plainum::lerp(t.z, _p_min.z, _p_max.z));
        }

        glm::vec<3, T> Offset(const glm::vec<3, T>& p) const
        {
            glm::vec<3, T> o = p - _p_min;
            if (_p_max.x > _p_min.x) o.x /= _p_max.x - _p_min.x;
            if (_p_max.y > _p_min.y) o.y /= _p_max.y - _p_min.y;
            if (_p_max.z > _p_min.z) o.z /= _p_max.z - _p_min.z;
            return o;
        }

        template <typename U>
        explicit operator Bounds3<U>() const
        {
            return Bounds3<U>((glm::vec<3, U>)_p_min, (glm::vec<3, U>)_p_max);
        }

        bool Hit(const Ray& ray, float& hitt0, float& hitt1) const;
        inline bool Hit(const Ray& ray, const glm::vec3& invDir, const int dirIsNeg[3]) const;

        friend std::ostream& operator<<(std::ostream& os, const Bounds3<T>& b)
        {
            os << "[ " << b._p_min << " - " << b._p_max << " ]";
            return os;
        }

    public:
        glm::vec<3, T> _p_min, _p_max;
    };

    template <typename T>
    inline Bounds3<T> UnionBounds(const Bounds3<T>& b, const glm::vec<3, T>& p)
    {
        Bounds3<T> ret;
        ret._p_min = min(b._p_min, p);
        ret._p_max = max(b._p_max, p);
        return ret;
    }

    template <typename T>
    inline Bounds3<T> UnionBounds(const Bounds3<T>& b1, const Bounds3<T>& b2)
    {
        Bounds3<T> ret;
        ret._p_min = min(b1._p_min, b2._p_min);
        ret._p_max = max(b1._p_max, b2._p_max);
        return ret;
    }

    template <typename T>
    inline Bounds3<T> Intersect(const Bounds3<T>& b1, const Bounds3<T>& b2)
    {
        // Important: assign to pMin/pMax directly and don't run the ABounds2()
        // constructor, since it takes min/max of the points passed to it.  In
        // turn, that breaks returning an invalid bound for the case where we
        // intersect non-overlapping bounds (as we'd like to happen).
        Bounds3<T> ret;
        ret._p_min = glm::max(b1._p_min, b2._p_min);
        ret._p_max = glm::min(b1._p_max, b2._p_max);
        return ret;
    }

    template <typename T>
    inline bool Overlaps(const Bounds3<T>& b1, const Bounds3<T>& b2)
    {
        bool x = (b1._p_max.x >= b2._p_min.x) && (b1._p_min.x <= b2._p_max.x);
        bool y = (b1._p_max.y >= b2._p_min.y) && (b1._p_min.y <= b2._p_max.y);
        bool z = (b1._p_max.z >= b2._p_min.z) && (b1._p_min.z <= b2._p_max.z);
        return (x && y && z);
    }

    template <typename T>
    inline bool Inside(const glm::vec<3, T>& p, const Bounds3<T>& b)
    {
        return (p.x >= b._p_min.x && p.x <= b._p_max.x && p.y >= b._p_min.y &&
            p.y <= b._p_max.y && p.z >= b._p_min.z && p.z <= b._p_max.z);
    }

	template <typename T>
	bool insideExclusive(const AVector2<T> &pt, const ABounds2<T> &b) 
	{
		return (pt.x >= b.m_pMin.x && pt.x < b.m_pMax.x && pt.y >= b.m_pMin.y && pt.y < b.m_pMax.y);
	}
    template<typename T>
    class Bounds2
    {
    public:

        Bounds2()
        {
            T minNum = std::numeric_limits<T>::lowest();
            T maxNum = std::numeric_limits<T>::max();
            _p_min = glm::vec<2, T>(maxNum, maxNum);
            _p_max = glm::vec<2, T>(minNum, minNum);
        }

        explicit Bounds2(const glm::vec<2, T>& p) : _p_min(p), _p_max(p) {}

        Bounds2(const glm::vec<2, T>& p1, const glm::vec<2, T>& p2)
        {
            _p_min = glm::vec<2, T>(glm::min(p1.x, p2.x), glm::min(p1.y, p2.y));
            _p_max = glm::vec<2, T>(glm::max(p1.x, p2.x), glm::max(p1.y, p2.y));
        }

        template <typename U>
        explicit operator Bounds2<U>() const
        {
            return Bounds2<U>((glm::vec<2, U>)_p_min, (glm::vec<2, U>)_p_max);
        }

        glm::vec<2, T> Diagonal() const { return _p_max - _p_min; }

        T Area() const
        {
            glm::vec<2, T> d = _p_max - _p_min;
            return (d.x * d.y);
        }

        int MaximumExtent() const {
            glm::vec<2, T> diag = diagonal();
            if (diag.x > diag.y)
                return 0;
            else
                return 1;
        }

        inline glm::vec<2, T>& operator[](int i)
        {
            DCHECK(i == 0 || i == 1);
            return (i == 0) ? _p_min : _p_max;
        }

        inline const glm::vec<2, T>& operator[](int i) const
        {
            DCHECK(i == 0 || i == 1);
            return (i == 0) ? _p_min : _p_max;
        }

        bool operator==(const Bounds2<T>& b) const { return b._p_min == pMin && b._p_max == pMax; }
        bool operator!=(const Bounds2<T>& b) const { return b._p_min != pMin || b._p_max != pMax; }

        glm::vec<2, T> Lerp(const glm::vec2& t) const
        {
            return glm::vec<2, T>(lerp(t.x, _p_min.x, _p_max.x), lerp(t.y, _p_min.y, _p_max.y));
        }

        glm::vec<2, T> Offset(const glm::vec<2, T>& p) const
        {
            glm::vec<2, T> o = p - _p_min;
            if (_p_max.x > _p_min.x) o.x /= _p_max.x - _p_min.x;
            if (_p_max.y > _p_min.y) o.y /= _p_max.y - _p_min.y;
            return o;
        }

        friend std::ostream& operator<<(std::ostream& os, const Bounds2<T>& b)
        {
            os << "[ " << b._p_min << " - " << b._p_max << " ]";
            return os;
        }

    public:
        glm::vec<2, T> _p_min, _p_max;

    };

    typedef Bounds2<float> Bounds2f;
    typedef Bounds2<int> Bounds2i;
    typedef Bounds3<float> Bounds3f;
    typedef Bounds3<int> Bounds3i;
    
    template <typename T>
    inline Bounds2<T> UnionBounds(const Bounds2<T>& b, const glm::vec<2, T>& p)
    {
        Bounds2<T> ret;
        ret._p_min = min(b._p_min, p);
        ret._p_max = max(b._p_max, p);
        return ret;
    }

    template <typename T>
    inline Bounds2<T> UnionBounds(const Bounds2<T>& b, const Bounds2<T>& b2)
    {
        Bounds2<T> ret;
        ret._p_min = min(b._p_min, b2._p_min);
        ret._p_max = max(b._p_max, b2._p_max);
        return ret;
    }

    template <typename T>
    inline Bounds2<T> Intersect(const Bounds2<T>& b1, const Bounds2<T>& b2)
    {
        // Important: assign to pMin/pMax directly and don't run the Bounds2()
        // constructor, since it takes min/max of the points passed to it.  In
        // turn, that breaks returning an invalid bound for the case where we
        // intersect non-overlapping bounds (as we'd like to happen).
        Bounds2<T> ret;
        ret._p_min = max(b1._p_min, b2._p_min);
        ret._p_max = min(b1._p_max, b2._p_max);
        return ret;
    }

    template <typename T>
    inline bool Overlaps(const Bounds2<T>& ba, const Bounds2<T>& bb)
    {
        bool x = (ba._p_max.x >= bb._p_min.x) && (ba._p_min.x <= bb._p_max.x);
        bool y = (ba._p_max.y >= bb._p_min.y) && (ba._p_min.y <= bb._p_max.y);
        return (x && y);
    }

    template <typename T>
    inline bool Inside(const glm::vec<2, T>& pt, const Bounds2<T>& b)
    {
        return (pt.x >= b._p_min.x && pt.x <= b._p_max.x && pt.y >= b._p_min.y && pt.y <= b._p_max.y);
    }

    template <typename T>
    bool InsideExclusive(const glm::vec<2, T>& pt, const Bounds2<T>& b)
    {
        return (pt.x >= b._p_min.x && pt.x < b._p_max.x&& pt.y >= b._p_min.y && pt.y < b._p_max.y);
    }

    class Bounds2iIterator : public std::forward_iterator_tag
    {
    public:
        Bounds2iIterator(const Bounds2i& b, const glm::ivec2& pt)
            : p(pt), bounds(&b) {}

        Bounds2iIterator operator++()
        {
            Advance();
            return *this;
        }

        Bounds2iIterator operator++(int)
        {
            Bounds2iIterator old = *this;
            Advance();
            return old;
        }

        bool operator==(const Bounds2iIterator& bi) const
        {
            return p == bi.p && bounds == bi.bounds;
        }

        bool operator!=(const Bounds2iIterator& bi) const
        {
            return p != bi.p || bounds != bi.bounds;
        }

        glm::ivec2 operator*() const { return p; }

    private:
        void Advance()
        {
            ++p.x;
            if (p.x == bounds->_p_max.x)
            {
                p.x = bounds->_p_min.x;
                ++p.y;
            }
        }

        glm::ivec2 p;
        const Bounds2i* bounds;
    };

    inline Bounds2iIterator begin(const Bounds2i& b)
    {
        return Bounds2iIterator(b, b._p_min);
    }

    inline Bounds2iIterator end(const Bounds2i& b)
    {
        // Normally, the ending point is at the minimum x value and one past
        // the last valid y value.
        glm::ivec2 pEnd(b._p_min.x, b._p_max.y);
        // However, if the bounds are degenerate, override the end point to
        // equal the start point so that any attempt to iterate over the bounds
        // exits out immediately.
        if (b._p_min.x >= b._p_max.x || b._p_min.y >= b._p_max.y)
            pEnd = b._p_min;
        return Bounds2iIterator(b, pEnd);
    }

      template <typename T>
	inline bool Bounds3<T>::Hit(const Ray &ray, float &hitt0, float &hitt1) const
	{
		float t0 = 0, t1 = ray.m_tMax;
		for (int i = 0; i < 3; ++i)
		{
			// Update interval for _i_th bounding box slab
			float invRayDir = 1 / ray.m_dir[i];
			float tNear = (_p_min[i] - ray.m_origin[i]) * invRayDir;
			float tFar = (_p_max[i] - ray.m_origin[i]) * invRayDir;

			// Update parametric interval from slab intersection $t$ values
			if (tNear > tFar)
			{
				std::swap(tNear, tFar);
			}

			// Update _tFar_ to ensure robust ray--bounds intersection
			tFar *= 1 + 2 * gamma(3);
			t0 = tNear > t0 ? tNear : t0;
			t1 = tFar < t1 ? tFar : t1;
			if (t0 > t1)
			{
				return false;
			}
		}
		hitt0 = t0;
		hitt1 = t1;
		return true;
	}

	template <typename T>
	inline bool Bounds3<T>::Hit(const Ray &ray, const glm::vec3 &invDir, const int dirIsNeg[3]) const
	{
		const ABounds3f &bounds = *this;
		// Check for ray intersection against $x$ and $y$ slabs
		float tMin = (bounds[dirIsNeg[0]].x - ray.m_origin.x) * invDir.x;
		float tMax = (bounds[1 - dirIsNeg[0]].x - ray.m_origin.x) * invDir.x;
		float tyMin = (bounds[dirIsNeg[1]].y - ray.m_origin.y) * invDir.y;
		float tyMax = (bounds[1 - dirIsNeg[1]].y - ray.m_origin.y) * invDir.y;

		// Update _tMax_ and _tyMax_ to ensure robust bounds intersection
		tMax *= 1 + 2 * gamma(3);
		tyMax *= 1 + 2 * gamma(3);
		if (tMin > tyMax || tyMin > tMax)
			return false;
		if (tyMin > tMin)
			tMin = tyMin;
		if (tyMax < tMax)
			tMax = tyMax;

		// Check for ray intersection against $z$ slab
		float tzMin = (bounds[dirIsNeg[2]].z - ray.m_origin.z) * invDir.z;
		float tzMax = (bounds[1 - dirIsNeg[2]].z - ray.om_origin.z) * invDir.z;

		// Update _tzMax_ to ensure robust bounds intersection
		tzMax *= 1 + 2 * gamma(3);
		if (tMin > tzMax || tzMin > tMax)
			return false;
		if (tzMin > tMin)
			tMin = tzMin;
		if (tzMax < tMax)
			tMax = tzMax;
		return (tMin < ray.m_tMax) && (tMax > 0);
	}


}

#endif