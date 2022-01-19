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
			m_pMin = glm::vec<3,T>(maxNum, maxNum, maxNum);
			m_pMax = glm::vec<3,T>(minNum, minNum, minNum);
		}

		explicit Bounds3(const glm::vec<3,T> &p) : m_pMin(p), m_pMax(p) {}

		Bounds3(const glm::vec<3,T> &p1, const glm::vec<3,T> &p2) :
			m_pMin(glm::min(p1.x, p2.x), glm::min(p1.y, p2.y), glm::min(p1.z, p2.z)),
			m_pMax(glm::max(p1.x, p2.x), glm::max(p1.y, p2.y), glm::max(p1.z, p2.z)) {}

		glm::vec<3,T> &operator[](int i) { return (i == 0) ? m_pMin : m_pMax; }
		const glm::vec<3,T> &operator[](int i) const { return (i == 0) ? m_pMin : m_pMax; }

		bool operator==(const Bounds3<T> &b) const { return b.m_pMin == m_pMin && b.m_pMax == m_pMax; }
		bool operator!=(const Bounds3<T> &b) const { return b.m_pMin != m_pMin || b.m_pMax != m_pMax; }

		glm::vec<3,T> Corner(int cor) const
		{
			DCHECK(cor >= 0 && cor < 8);
			return glm::vec<3,T>((*this)[(cor & 1)].x, (*this)[(cor & 2) ? 1 : 0].y, (*this)[(cor & 4) ? 1 : 0].z);
		}

		glm::vec<3,T> Diagonal() const { return m_pMax - m_pMin; }

		T SurfaceArea() const
		{
			glm::vec<3,T> d = Diagonal();
			return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
		}

		T Volume() const
		{
			glm::vec<3,T> d = Diagonal();
			return d.x * d.y * d.z;
		}

		int MaximumExtent() const
		{
			glm::vec<3,T> d = Diagonal();
			if (d.x > d.y && d.x > d.z)
				return 0;
			else if (d.y > d.z)
				return 1;
			else
				return 2;
		}

		glm::vec<3,T> Lerp(const glm::vec3 &t) const
		{
			return glm::vec<3,T>(
				plainum::lerp(t.x, m_pMin.x, m_pMax.x),
				plainum::lerp(t.y, m_pMin.y, m_pMax.y),
				plainum::lerp(t.z, m_pMin.z, m_pMax.z));
		}

		glm::vec<3,T> Offset(const glm::vec<3,T> &p) const
		{
			glm::vec<3,T> o = p - m_pMin;
			if (m_pMax.x > m_pMin.x) o.x /= m_pMax.x - m_pMin.x;
			if (m_pMax.y > m_pMin.y) o.y /= m_pMax.y - m_pMin.y;
			if (m_pMax.z > m_pMin.z) o.z /= m_pMax.z - m_pMin.z;
			return o;
		}

		template <typename U>
		explicit operator Bounds3<U>() const
		{
			return Bounds3<U>((glm::vec<3,U>)m_pMin, (glm::vec<3,U>)m_pMax);
		}

		bool Hit(const Ray &ray, float &hitt0, float &hitt1) const;
		inline bool Hit(const Ray &ray, const glm::vec3 &invDir, const int dirIsNeg[3]) const;

		friend std::ostream &operator<<(std::ostream &os, const Bounds3<T> &b)
		{
			os << "[ " << b.m_pMin << " - " << b.m_pMax << " ]";
			return os;
		}

	public:
		glm::vec<3,T> m_pMin, m_pMax;
    };

   template <typename T>
	inline Bounds3<T> UnionBounds(const Bounds3<T> &b, const glm::vec<3,T> &p)
	{
		Bounds3<T> ret;
		ret.m_pMin = min(b.m_pMin, p);
		ret.m_pMax = max(b.m_pMax, p);
		return ret;
	}

	template <typename T>
	inline Bounds3<T> UnionBounds(const Bounds3<T> &b1, const Bounds3<T> &b2)
	{
		Bounds3<T> ret;
		ret.m_pMin = min(b1.m_pMin, b2.m_pMin);
		ret.m_pMax = max(b1.m_pMax, b2.m_pMax);
		return ret;
	}

	template <typename T>
	inline Bounds3<T> Intersect(const Bounds3<T> &b1, const Bounds3<T> &b2)
	{
		// Important: assign to pMin/pMax directly and don't run the ABounds2()
		// constructor, since it takes min/max of the points passed to it.  In
		// turn, that breaks returning an invalid bound for the case where we
		// intersect non-overlapping bounds (as we'd like to happen).
		Bounds3<T> ret;
		ret.m_pMin = glm::max(b1.m_pMin, b2.m_pMin);
		ret.m_pMax = glm::min(b1.m_pMax, b2.m_pMax);
		return ret;
	}

	template <typename T>
	inline bool Overlaps(const Bounds3<T> &b1, const Bounds3<T> &b2)
	{
		bool x = (b1.m_pMax.x >= b2.m_pMin.x) && (b1.m_pMin.x <= b2.m_pMax.x);
		bool y = (b1.m_pMax.y >= b2.m_pMin.y) && (b1.m_pMin.y <= b2.m_pMax.y);
		bool z = (b1.m_pMax.z >= b2.m_pMin.z) && (b1.m_pMin.z <= b2.m_pMax.z);
		return (x && y && z);
	}

	template <typename T>
	inline bool Inside(const glm::vec<3,T> &p, const Bounds3<T> &b)
	{
		return (p.x >= b.m_pMin.x && p.x <= b.m_pMax.x && p.y >= b.m_pMin.y &&
			p.y <= b.m_pMax.y && p.z >= b.m_pMin.z && p.z <= b.m_pMax.z);
	}

    
	template<typename T>
	class Bounds2
	{
	public:

		Bounds2()
		{
			T minNum = std::numeric_limits<T>::lowest();
			T maxNum = std::numeric_limits<T>::max();
			m_pMin = glm::vec<2,T>(maxNum, maxNum);
			m_pMax = glm::vec<2,T>(minNum, minNum);
		}

		explicit Bounds2(const glm::vec<2,T> &p) : m_pMin(p), m_pMax(p) {}

		Bounds2(const glm::vec<2,T> &p1, const glm::vec<2,T> &p2)
		{
			m_pMin = glm::vec<2,T>(glm::min(p1.x, p2.x), glm::min(p1.y, p2.y));
			m_pMax = glm::vec<2,T>(glm::max(p1.x, p2.x), glm::max(p1.y, p2.y));
		}

		template <typename U>
		explicit operator Bounds2<U>() const
		{
			return Bounds2<U>((glm::vec<2,U>)m_pMin, (glm::vec<2,U>)m_pMax);
		}

		glm::vec<2,T> Diagonal() const { return m_pMax - m_pMin; }

		T Area() const
		{
			glm::vec<2,T> d = m_pMax - m_pMin;
			return (d.x * d.y);
		}

		int MaximumExtent() const {
			glm::vec<2,T> diag = diagonal();
			if (diag.x > diag.y)
				return 0;
			else
				return 1;
		}

		inline glm::vec<2,T> &operator[](int i) 
		{ 
			DCHECK(i == 0 || i == 1);
			return (i == 0) ? m_pMin : m_pMax; 
		}

		inline const glm::vec<2,T> &operator[](int i) const 
		{
			DCHECK(i == 0 || i == 1);
			return (i == 0) ? m_pMin : m_pMax; 
		}

		bool operator==(const Bounds2<T> &b) const { return b.m_pMin == pMin && b.m_pMax == pMax; }
		bool operator!=(const Bounds2<T> &b) const { return b.m_pMin != pMin || b.m_pMax != pMax; }

		glm::vec<2,T> Lerp(const glm::vec2f &t) const
		{
			return glm::vec<2,T>(lerp(t.x, m_pMin.x, m_pMax.x), lerp(t.y, m_pMin.y, m_pMax.y));
		}

		glm::vec<2,T> Offset(const glm::vec<2,T> &p) const
		{
			glm::vec<2,T> o = p - m_pMin;
			if (m_pMax.x > m_pMin.x) o.x /= m_pMax.x - m_pMin.x;
			if (m_pMax.y > m_pMin.y) o.y /= m_pMax.y - m_pMin.y;
			return o;
		}

		friend std::ostream &operator<<(std::ostream &os, const Bounds2<T> &b)
		{
			os << "[ " << b.m_pMin << " - " << b.m_pMax << " ]";
			return os;
		}

	public:
		glm::vec<2,T> m_pMin, m_pMax;

    };

    typedef Bounds2<float> ABounds2f;
	typedef Bounds2<int> Bounds2i;
	template <typename T>
	inline Bounds2<T> UnionBounds(const Bounds2<T> &b, const glm::vec<2,T> &p)
	{
		Bounds2<T> ret;
		ret.m_pMin = min(b.m_pMin, p);
		ret.m_pMax = max(b.m_pMax, p);
		return ret;
	}

	template <typename T>
	inline Bounds2<T> UnionBounds(const Bounds2<T> &b, const Bounds2<T> &b2)
	{
		Bounds2<T> ret;
		ret.m_pMin = min(b.m_pMin, b2.m_pMin);
		ret.m_pMax = max(b.m_pMax, b2.m_pMax);
		return ret;
	}

	template <typename T>
	inline Bounds2<T> Intersect(const Bounds2<T> &b1, const Bounds2<T> &b2)
	{
		// Important: assign to pMin/pMax directly and don't run the Bounds2()
		// constructor, since it takes min/max of the points passed to it.  In
		// turn, that breaks returning an invalid bound for the case where we
		// intersect non-overlapping bounds (as we'd like to happen).
		Bounds2<T> ret;
		ret.m_pMin = max(b1.m_pMin, b2.m_pMin);
		ret.m_pMax = min(b1.m_pMax, b2.m_pMax);
		return ret;
	}

	template <typename T>
	inline bool overlaps(const Bounds2<T> &ba, const Bounds2<T> &bb)
	{
		bool x = (ba.m_pMax.x >= bb.m_pMin.x) && (ba.m_pMin.x <= bb.m_pMax.x);
		bool y = (ba.m_pMax.y >= bb.m_pMin.y) && (ba.m_pMin.y <= bb.m_pMax.y);
		return (x && y);
	}

	template <typename T>
	inline bool inside(const glm::vec<2,T> &pt, const Bounds2<T> &b)
	{
		return (pt.x >= b.m_pMin.x && pt.x <= b.m_pMax.x && pt.y >= b.m_pMin.y && pt.y <= b.m_pMax.y);
	}

	template <typename T>
	bool insideExclusive(const glm::vec<2,T> &pt, const Bounds2<T> &b) 
	{
		return (pt.x >= b.m_pMin.x && pt.x < b.m_pMax.x && pt.y >= b.m_pMin.y && pt.y < b.m_pMax.y);
	}

	class Bounds2iIterator : public std::forward_iterator_tag 
	{
	public:
		Bounds2iIterator(const Bounds2i &b, const glm::ivec2 &pt)
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

		bool operator==(const Bounds2iIterator &bi) const 
		{
			return p == bi.p && bounds == bi.bounds;
		}

		bool operator!=(const Bounds2iIterator &bi) const 
		{
			return p != bi.p || bounds != bi.bounds;
		}

		glm::ivec2 operator*() const { return p; }

	private:
		void Advance() 
		{
			++p.x;
			if (p.x == bounds->m_pMax.x) 
			{
				p.x = bounds->m_pMin.x;
				++p.y;
			}
		}

		glm::ivec2 p;
		const Bounds2i *bounds;
	};

	inline Bounds2iIterator begin(const Bounds2i &b) 
	{
		return Bounds2iIterator(b, b.m_pMin);
	}

	inline Bounds2iIterator end(const Bounds2i &b) 
	{
		// Normally, the ending point is at the minimum x value and one past
		// the last valid y value.
		glm::ivec2 pEnd(b.m_pMin.x, b.m_pMax.y);
		// However, if the bounds are degenerate, override the end point to
		// equal the start point so that any attempt to iterate over the bounds
		// exits out immediately.
		if (b.m_pMin.x >= b.m_pMax.x || b.m_pMin.y >= b.m_pMax.y)
			pEnd = b.m_pMin;
		return Bounds2iIterator(b, pEnd);
	}

}

#endif