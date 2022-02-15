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

#ifndef CORE_RAY_H_
#define CORE_RAY_H_

#include <glm/glm.hpp>
#include <core/utilities.h>

namespace platinum
{
    class Ray
    {
    public:
    public:
        Ray() : _t_max(Infinity) {}

        Ray(const Vector3f &o, const Vector3f &d, float tMax = Infinity)
            : _origin(o), _direction(normalize(d)), _t_max(tMax) {}

        const Vector3f &GetOrigin() const { return _origin; }
        
        const Vector3f &GetDirection() const { return _direction; }

        Vector3f GetPointAt(float t) const { return _origin + _direction * t; }

        friend std::ostream &operator<<(std::ostream &os, const Ray &r)
        {
            os << "[o=" << r._origin << ", d=" << r._direction << ", tMax=" << r._t_max << "]";
            return os;
        }

    public:
        Vector3f _origin;
        Vector3f _direction;
        mutable float _t_max;
    };

} // namespace platinum

#endif
