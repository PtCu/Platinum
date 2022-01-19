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
    template <typename T>
	inline bool Bounds3<T>::Hit(const Ray &ray, float &hitt0, float &hitt1) const
	{
		float t0 = 0, t1 = ray.m_tMax;
		for (int i = 0; i < 3; ++i)
		{
			// Update interval for _i_th bounding box slab
			float invRayDir = 1 / ray.m_dir[i];
			float tNear = (m_pMin[i] - ray.m_origin[i]) * invRayDir;
			float tFar = (m_pMax[i] - ray.m_origin[i]) * invRayDir;

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
