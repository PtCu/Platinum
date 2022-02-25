

#include <core/shape.h>
#include <core/interaction.h>
#include <math/transform.h>
#include <glm/gtx/norm.hpp>
#include <math/bounds.h>

namespace platinum
{

    bool Shape::Hit(const Ray &ray) const
    {
        float t_hit = ray._t_max;
        SurfaceInteraction inter;
        return Hit(ray, t_hit, inter);
    }

    void Shape::SetTransform(Transform *object2world, Transform *world2object)
    {
        _object2world = object2world;
        _world2object = world2object;
    }

    Bounds3f Shape::WorldBound() const
    {
        return _object2world->ExecOn(this->ObjectBound());
    }

    Interaction Shape::Sample(const Interaction &ref, const Vector2f &u, float &pdf) const
    {
        Interaction inter = Sample(u, pdf);
        Vector3f wi = inter.p - ref.p;
        if (glm::dot(wi, wi) == 0)
        {
            pdf = 0;
        }
        else
        {
            wi = glm::normalize(wi);
            pdf *= glm::distance2(ref.p, inter.p) / glm::abs(glm::dot(inter.n, -wi));
            if (std::isinf(pdf))
                pdf = 0.f;
        }

        return inter;
    }

    float Shape::Pdf(const Interaction &ref, const Vector3f &wi) const
    {
        Ray ray = ref.SpawnRay(wi);
        float t_hit;
        SurfaceInteraction inter_light;
        if (!Hit(ray, t_hit, inter_light))
            return 0.f;
        float pdf = glm::distance2(ref.p, inter_light.p) / (glm::abs(glm::dot(inter_light.n, -wi)) * Area());
        if (std::isinf(pdf))
            pdf = 0.f;
        return 1.f;
    }

    float Shape::SolidAngle(const Vector3f &p, int nSamples) const
    {
        return 1.0f;
    }
}