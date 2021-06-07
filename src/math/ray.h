#ifndef MATH_RAY_H_
#define MATH_RAY_H_

#include "core/platinum.h"

PLAITNUM_BEGIN
class Ray
{
public:
    Ray() : m_tMax(aInfinity) {}

    Ray(const Vector3f &o, const Vector3f &d, Float tMax = aInfinity)
        : m_origin(o), m_dir(normalize(d)), m_tMax(tMax) {}

    const Vector3f &origin() const { return m_origin; }
    const Vector3f &direction() const { return m_dir; }

    Vector3f operator()(Float t) const { return m_origin + m_dir * t; }

    friend std::ostream &operator<<(std::ostream &os, const Ray &r)
    {
        os << "[o=" << r.m_origin << ", d=" << r.m_dir << ", tMax=" << r.m_tMax << "]";
        return os;
    }

public:
    Vector3f m_origin;
    Vector3f m_dir;
    mutable Float m_tMax;
};
PLATINUM_END

#endif