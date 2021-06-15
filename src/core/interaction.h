#ifndef CORE_INTERACTION_H_
#define CORE_INTERACTION_H_

#include "platinum.h"
#include "math_utils.h"

namespace platinum{
class Interaction
{
public:
    Interaction() = default;
    Interaction(const Vector3f &p) : p(p) {}
    Interaction(const Vector3f &p, const Vector3f &wo) : p(p), wo(normalize(wo)) {}
    Interaction(const Vector3f &p, const Vector3f &n, const Vector3f &wo)
        : p(p), wo(normalize(wo)), n(n) {}

    inline Ray spawnRay(const Vector3f &d) const
    {
        Vector3f o = p;
        return Ray(o, d, Infinity);
    }

    inline Ray spawnRayTo(const Vector3f &p2) const
    {
        Vector3f origin = p;
        return Ray(origin, p2 - p, 1 - ShadowEpsilon);
    }

    inline Ray spawnRayTo(const Interaction &it) const
    {
        Vector3f origin = p;
        Vector3f target = it.p;
        Vector3f d = target - origin;
        return Ray(origin, d, 1 - ShadowEpsilon);
    }

public:
    Vector3f p;  //surface point
    Vector3f wo; //outgoing direction
    Vector3f n;  //normal vector
};

class SurfaceInteraction final : public Interaction
{
public:
    SurfaceInteraction() = default;
    SurfaceInteraction(const Vector3f &p, const Vector2f &uv, const Vector3f &wo,
                        const Vector3f &dpdu, const Vector3f &dpdv, const Shape *sh);

    Spectrum Le(const Vector3f &w) const;

    void computeScatteringFunctions(const Ray &ray, MemoryArena &arena,
                                    bool allowMultipleLobes = false, TransportMode mode = TransportMode::Radiance);

public:
    Vector2f uv;
    Vector3f dpdu, dpdv;

    BSDF *bsdf = nullptr;

    const Shape *shape = nullptr;
    const Hitable *hitable = nullptr;
};
}
#endif