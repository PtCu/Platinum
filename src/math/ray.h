

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
