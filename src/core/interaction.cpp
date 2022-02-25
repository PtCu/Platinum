

#include <core/interaction.h>
#include <core/spectrum.h>
#include <core/primitive.h>
namespace platinum
{

    SurfaceInteraction::SurfaceInteraction(const Vector3f &p, const Vector2f &uv, const Vector3f &wo,
                                           const Vector3f &dpdu, const Vector3f &dpdv, const Shape *sh)
        : Interaction(p, glm::normalize(glm::cross(dpdu, dpdv)), wo), _uv(uv), _dpdu(dpdu), _dpdv(dpdv), _shape(sh)
    {
    }

    Spectrum SurfaceInteraction::Le(const Vector3f &w) const
    {
        const AreaLight *area = _hitable->GetAreaLight();
        if (area)
            return area->L(*this, w);
        else
            return Spectrum(0.f);
    }

    void SurfaceInteraction::ComputeScatteringFunctions(const Ray &ray, MemoryArena &arena)
    {
        _hitable->ComputeScatteringFunctions(*this, arena);
    }
}