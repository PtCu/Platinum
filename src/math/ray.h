#ifndef MATH_RAY_H_
#define MATH_RAY_H_

#include "core/platinum.h"

class Ray
{
private:
    /* data */
public:
    Ray() : tMax(Infinity), time(0.f), medium(nullptr), tMin(RayEpsilon) {}
    Ray(const Point3f &ori, const Vector3f &dir, Float tMax = Infinity,
        Float time = 0.f, const Medium *medium = nullptr,
        Float tMin = RayEpsilon)
        : ori(ori),
          dir(dir),
          tMax(tMax),
          time(time),
          medium(medium),
          tMin(tMin)
    {
    }
    // 个人风格，不喜欢重载括号，改用at
    Point3f at(Float t) const
    {
        return ori + dir * t;
    }

    bool isValid() const
    {
        return !dir.isZero();
    }

    bool hasNaNs() const
    {
        return (ori.hasNaNs() || dir.hasNaNs() || isNaN(tMax));
    }

    friend std::ostream &operator<<(std::ostream &os, const Ray &r)
    {
        os << "[o=" << r.ori << ", d=" << r.dir << ", tMax=" << r.tMax
           << ", time=" << r.time << "]";
        return os;
    }
    // 起点
    Point3f ori;
    // 方向，单位向量
    Vector3f dir;
    // 发射的时间，用于做motion blur
    Float time;
    // 光线的最远距离
    Float tMax;
    Float tMin;
    const Medium *medium;
};

#endif