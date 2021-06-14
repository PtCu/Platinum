#ifndef MATH_UTILS_H_
#define MATH_UTILS_H_

#include "core/platinum.h"
#include "glm/common.hpp"
#include "glm/gtc/quaternion.hpp"

#include <ctime>
#include <iterator>

PLAITNUM_BEGIN

typedef unsigned int uint;

template <typename T>
inline bool isNaN(const T x) { return std::isnan(x); }

template <>
inline bool isNaN(const int x) { return false; }

//-------------------------------------------Vector/Point/Normal-------------------------------------

template <typename T>
using Vector2 = glm::vec<2, T>;

template <typename T>
using Vector3 = glm::vec<3, T>;

typedef Vector2<Float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Vector3<Float> Vector3f;
typedef Vector3<int> AVector3i;

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Vector2<T> &v)
{
    os << "[ " << v.x << ", " << v.y << " ]";
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Vector3<T> &v)
{
    os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
    return os;
}

//-------------------------------------------Matrix/Quaterion-------------------------------------

using Matrix4x4 = glm::mat<4, 4, Float>;
using Quaterion = glm::qua<Float>;

inline Matrix4x4 transpose(const Matrix4x4 &m) { return glm::transpose(m); }
inline Matrix4x4 mul(const Matrix4x4 &m1, const Matrix4x4 &m2) { return m1 * m2; }
inline Matrix4x4 inverse(const Matrix4x4 &m) { return glm::inverse(m); }

inline Matrix4x4 toMatrix4x4(const Quaterion &q) { return glm::mat4_cast(q); }

template <typename T>
inline Float dot(const Vector2<T> &v1, const Vector2<T> &v2) { return glm::dot(v1, v2); }

template <typename T>
inline Float absDot(const Vector2<T> &v1, const Vector2<T> &v2) { return glm::abs(dot(v1, v2)); }

template <typename T>
inline Vector2<T> normalize(const Vector2<T> &v) { return glm::normalize(v); }

template <typename T>
Vector2<T> abs(const Vector2<T> &v) { return glm::abs(v); }

template <typename T>
inline Float length(const Vector3<T> &p) { return glm::length(p); }

template <typename T>
inline Float lengthSquared(const Vector3<T> &p) { return glm::dot(p, p); }

template <typename T>
inline Float distance(const Vector3<T> &p1, const Vector3<T> &p2) { return glm::length(p1 - p2); }

template <typename T>
inline Float distanceSquared(const Vector3<T> &p1, const Vector3<T> &p2) { return glm::dot(p1 - p2, p1 - p2); }

template <typename T>
inline Vector3<T> lerp(Float t, const Vector3<T> &p0, const Vector3<T> &p1) { return (1 - t) * p0 + t * p1; }

template <typename T>
inline Vector3<T> min(const Vector3<T> &p1, const Vector3<T> &p2)
{
    return Vector3<T>(glm::min(p1.x, p2.x), glm::min(p1.y, p2.y), glm::min(p1.z, p2.z));
}

template <typename T>
inline Vector3<T> max(const Vector3<T> &p1, const Vector3<T> &p2)
{
    return Vector3<T>(glm::max(p1.x, p2.x), glm::max(p1.y, p2.y), glm::max(p1.z, p2.z));
}

template <typename T>
inline Vector3<T> floor(const Vector3<T> &p)
{
    return Vector3<T>(glm::floor(p.x), glm::floor(p.y), glm::floor(p.z));
}

template <typename T>
inline Vector3<T> ceil(const Vector3<T> &p)
{
    return Vector3<T>(glm::ceil(p.x), glm::ceil(p.y), glm::ceil(p.z));
}

template <typename T>
inline Vector3<T> abs(const Vector3<T> &p)
{
    return Vector3<T>(glm::abs(p.x), glm::abs(p.y), glm::abs(p.z));
}

template <typename T>
inline Float distance(const Vector2<T> &p1, const Vector2<T> &p2) { return glm::length(p1, p2); }

template <typename T>
inline Float distanceSquared(const Vector2<T> &p1, const Vector2<T> &p2) { return glm::dot(p1 - p2, p1 - p2); }

template <typename T>
inline Vector2<T> floor(const Vector2<T> &p) { return Vector2<T>(glm::floor(p.x), glm::floor(p.y)); }

template <typename T>
inline Vector2<T> ceil(const Vector2<T> &p) { return Vector2<T>(glm::ceil(p.x), glm::ceil(p.y)); }

template <typename T>
inline Vector2<T> lerp(Float t, const Vector2<T> &v0, const Vector2<T> &v1) { return (1 - t) * v0 + t * v1; }

template <typename T>
inline Vector2<T> min(const Vector2<T> &pa, const Vector2<T> &pb) { return Vector2<T>(glm::min(pa.x, pb.x), glm::min(pa.y, pb.y)); }

template <typename T>
inline Vector2<T> max(const Vector2<T> &pa, const Vector2<T> &pb) { return Vector2<T>(glm::max(pa.x, pb.x), glm::max(pa.y, pb.y)); }

template <typename T>
inline T dot(const Vector3<T> &n1, const Vector3<T> &v2) { return glm::dot(n1, v2); }

template <typename T>
inline T absDot(const Vector3<T> &n1, const Vector3<T> &v2) { return glm::abs(glm::dot(n1, v2)); }

template <typename T>
inline Vector3<T> cross(const Vector3<T> &v1, const Vector3<T> &v2) { return glm::cross(v1, v2); }

template <typename T>
inline Vector3<T> normalize(const Vector3<T> &v) { return glm::normalize(v); }

template <typename T>
inline T minComponent(const Vector3<T> &v) { return glm::min(v.x, glm::min(v.y, v.z)); }

template <typename T>
inline T maxComponent(const Vector3<T> &v) { return glm::max(v.x, glm::max(v.y, v.z)); }

template <typename T>
inline int maxDimension(const Vector3<T> &v) { return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2); }

template <typename T>
inline Vector3<T> permute(const Vector3<T> &v, int x, int y, int z) { return Vector3<T>(v[x], v[y], v[z]); }

inline Float lerp(Float t, Float v1, Float v2) { return (1 - t) * v1 + t * v2; }
inline Float gamma(int n) { return (n * MachineEpsilon) / (1 - n * MachineEpsilon); }

inline Float gammaCorrect(Float value)
{
    if (value <= 0.0031308f)
        return 12.92f * value;
    return 1.055f * glm::pow(value, (Float)(1.f / 2.4f)) - 0.055f;
}

inline Float inverseGammaCorrect(Float value)
{
    if (value <= 0.04045f)
        return value * 1.f / 12.92f;
    return glm::pow((value + 0.055f) * 1.f / 1.055f, (Float)2.4f);
}

template <typename T, typename U, typename V>
inline T clamp(T val, U low, V high)
{
    if (val < low)
        return low;
    else if (val > high)
        return high;
    else
        return val;
}
template <typename T>
inline Vector3<T> faceforward(const Vector3<T> &n, const Vector3<T> &v) { return (dot(n, v) < 0.f) ? -n : n; }

template <typename T>
inline void coordinateSystem(const Vector3<T> &v1, Vector3<T> &v2, Vector3<T> &v3)
{
    if (glm::abs(v1.x) > glm::abs(v1.y))
        v2 = Vector3<T>(-v1.z, 0, v1.x) / glm::sqrt(v1.x * v1.x + v1.z * v1.z);
    else
        v2 = Vector3<T>(0, v1.z, -v1.y) / glm::sqrt(v1.y * v1.y + v1.z * v1.z);
    v3 = cross(v1, v2);
}

inline Vector3f sphericalDirection(Float sinTheta, Float cosTheta, Float phi)
{
    return Vector3f(sinTheta * glm::cos(phi), sinTheta * glm::sin(phi), cosTheta);
}

inline Vector3f sphericalDirection(Float sinTheta, Float cosTheta, Float phi,
                                   const Vector3f &x, const Vector3f &y, const Vector3f &z)
{
    return sinTheta * glm::cos(phi) * x + sinTheta * glm::sin(phi) * y + cosTheta * z;
}

template <typename T>
inline Bounds3<T> unionBounds(const Bounds3<T> &b, const Vector3<T> &p)
{
    Bounds3<T> ret;
    ret.m_pMin = min(b.m_pMin, p);
    ret.m_pMax = max(b.m_pMax, p);
    return ret;
}

template <typename T>
inline Bounds3<T> unionBounds(const Bounds3<T> &b1, const Bounds3<T> &b2)
{
    Bounds3<T> ret;
    ret.m_pMin = min(b1.m_pMin, b2.m_pMin);
    ret.m_pMax = max(b1.m_pMax, b2.m_pMax);
    return ret;
}

template <typename T>
inline Bounds3<T> intersect(const Bounds3<T> &b1, const Bounds3<T> &b2)
{
    // Important: assign to pMin/pMax directly and don't run the Bounds2()
    // constructor, since it takes min/max of the points passed to it.  In
    // turn, that breaks returning an invalid bound for the case where we
    // intersect non-overlapping bounds (as we'd like to happen).
    Bounds3<T> ret;
    ret.m_pMin = max(b1.m_pMin, b2.m_pMin);
    ret.m_pMax = Min(b1.m_pMax, b2.m_pMax);
    return ret;
}

template <typename T>
inline bool overlaps(const Bounds3<T> &b1, const Bounds3<T> &b2)
{
    bool x = (b1.m_pMax.x >= b2.m_pMin.x) && (b1.m_pMin.x <= b2.m_pMax.x);
    bool y = (b1.m_pMax.y >= b2.m_pMin.y) && (b1.m_pMin.y <= b2.m_pMax.y);
    bool z = (b1.m_pMax.z >= b2.m_pMin.z) && (b1.m_pMin.z <= b2.m_pMax.z);
    return (x && y && z);
}

template <typename T>
inline bool inside(const Vector3<T> &p, const Bounds3<T> &b)
{
    return (p.x >= b.m_pMin.x && p.x <= b.m_pMax.x && p.y >= b.m_pMin.y &&
            p.y <= b.m_pMax.y && p.z >= b.m_pMin.z && p.z <= b.m_pMax.z);
}

template <typename T>
inline Bounds2<T> unionBounds(const Bounds2<T> &b, const Vector2<T> &p)
{
    Bounds2<T> ret;
    ret.m_pMin = min(b.m_pMin, p);
    ret.m_pMax = max(b.m_pMax, p);
    return ret;
}

template <typename T>
inline Bounds2<T> unionBounds(const Bounds2<T> &b, const Bounds2<T> &b2)
{
    Bounds2<T> ret;
    ret.m_pMin = min(b.m_pMin, b2.m_pMin);
    ret.m_pMax = max(b.m_pMax, b2.m_pMax);
    return ret;
}

template <typename T>
inline Bounds2<T> intersect(const Bounds2<T> &b1, const Bounds2<T> &b2)
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
inline bool inside(const Vector2<T> &pt, const Bounds2<T> &b)
{
    return (pt.x >= b.m_pMin.x && pt.x <= b.m_pMax.x && pt.y >= b.m_pMin.y && pt.y <= b.m_pMax.y);
}

template <typename T>
bool insideExclusive(const Vector2<T> &pt, const Bounds2<T> &b)
{
    return (pt.x >= b.m_pMin.x && pt.x < b.m_pMax.x && pt.y >= b.m_pMin.y && pt.y < b.m_pMax.y);
}

template <typename T>
inline bool Bounds3<T>::hit(const Ray &ray, Float &hitt0, Float &hitt1) const
{
    Float t0 = 0, t1 = ray.m_tMax;
    for (int i = 0; i < 3; ++i)
    {
        // Update interval for _i_th bounding box slab
        Float invRayDir = 1 / ray.m_dir[i];
        Float tNear = (m_pMin[i] - ray.m_origin[i]) * invRayDir;
        Float tFar = (m_pMax[i] - ray.m_origin[i]) * invRayDir;

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
inline bool Bounds3<T>::hit(const Ray &ray, const Vector3f &invDir, const int dirIsNeg[3]) const
{
    const Bounds3f &bounds = *this;
    // Check for ray intersection against $x$ and $y$ slabs
    Float tMin = (bounds[dirIsNeg[0]].x - ray.m_origin.x) * invDir.x;
    Float tMax = (bounds[1 - dirIsNeg[0]].x - ray.m_origin.x) * invDir.x;
    Float tyMin = (bounds[dirIsNeg[1]].y - ray.m_origin.y) * invDir.y;
    Float tyMax = (bounds[1 - dirIsNeg[1]].y - ray.m_origin.y) * invDir.y;

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
    Float tzMin = (bounds[dirIsNeg[2]].z - ray.m_origin.z) * invDir.z;
    Float tzMax = (bounds[1 - dirIsNeg[2]].z - ray.om_origin.z) * invDir.z;

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

template <typename Predicate>
int findInterval(int size, const Predicate &pred)
{
    int first = 0, len = size;
    while (len > 0)
    {
        int half = len >> 1, middle = first + half;
        // Bisect range based on value of _pred_ at _middle_
        if (pred(middle))
        {
            first = middle + 1;
            len -= half + 1;
        }
        else
        {
            len = half;
        }
    }
    return clamp(first - 1, 0, size - 2);
}

inline Vector3f reflect(const Vector3f &wo, const Vector3f &n) { return -wo + 2 * dot(wo, n) * n; }

inline bool refract(const Vector3f &wi, const Vector3f &n, Float eta, Vector3f &wt)
{
    // Compute $\cos \theta_\roman{t}$ using Snell's law
    Float cosThetaI = dot(n, wi);
    Float sin2ThetaI = glm::max(Float(0), Float(1 - cosThetaI * cosThetaI));
    Float sin2ThetaT = eta * eta * sin2ThetaI;

    // Handle total internal reflection for transmission
    if (sin2ThetaT >= 1)
        return false;
    Float cosThetaT = glm::sqrt(1 - sin2ThetaT);
    wt = eta * -wi + (eta * cosThetaI - cosThetaT) * Vector3f(n);
    return true;
}
PLATINUM_END

#endif