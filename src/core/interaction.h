

#ifndef CORE_INTERSECTION_H_
#define CORE_INTERSECTION_H_

#include <core/utilities.h>
#include <math/ray.h>

namespace platinum
{

    //SurfaceInteraction和MediumInteraction的基类
    class Interaction
    {
    public:
        Interaction() = default;

        explicit Interaction(const Vector3f &p) : p(p) {}

        Interaction(const Vector3f &p, const Vector3f &wo) : p(p), wo(glm::normalize(wo)) {}

        Interaction(const Vector3f &p, const Vector3f &n, const Vector3f &wo)
            : p(p), wo(glm::normalize(wo)), n(n) {}

        inline Ray SpawnRay(const Vector3f &d) const
        {
            Vector3f o = p;
            return Ray(o, d, Infinity);
        }

        inline Ray SpawnRayTo(const Vector3f &p2) const
        {
            Vector3f origin = p;
            return Ray(origin, p2 - p, 1.f - ShadowEpsilon);
        }

        inline Ray SpawnRayTo(const Interaction &it) const
        {
            Vector3f origin = p;
            Vector3f target = it.p;
            Vector3f d = target - origin;
            return Ray(origin, d, 1.f - ShadowEpsilon);
        }

        Vector3f p;  //surface point
        Vector3f wo; //outgoing direction
        Vector3f n;  //normal vector
    };

    /**
     * @brief Having this abstraction lets most of the system work with points on surfaces
     * without needing to consider the particular type of geometric shape the points lie on
     */
    class SurfaceInteraction final : public Interaction
    {
    public:
        SurfaceInteraction() = default;

        SurfaceInteraction(const Vector3f &p, const Vector2f &uv, const Vector3f &wo,
                           const Vector3f &dpdu, const Vector3f &dpdv, const Shape *sh);

        /**
         * @brief Compute the emitted radiance at a surface point intersected by a ray.
         * 
         * @param w 
         * @return Spectrum 
         */
        Spectrum Le(const Vector3f &w) const;

        void ComputeScatteringFunctions(const Ray &ray, MemoryArena &arena);

    public:
        BSDF *_bsdf{nullptr};
        const Shape *_shape{nullptr};
        const Primitive *_hitable{nullptr};

        /**
         * @brief   (u,v)是p点参数化后的表面坐标（如纹理坐标）
         *          dpdu和dpdv是p在u,v方向的微分。二者不必正交
         */
        Vector2f _uv;
        Vector3f _dpdu, _dpdv;
    };

}
#endif
