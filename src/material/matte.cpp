

#include <material/matte.h>

namespace platinum
{

    REGISTER_CLASS(Matte, "Matte");

    Matte::Matte(const PropertyTree &node)
    {
        Vector3f spectrum = node.Get<Vector3f>("R");
        _Kr = Spectrum::fromRGB(spectrum);
    }

    Matte::Matte() : _Kr(Spectrum(0.73, 0.73, 0.73))
    {
    }

    Matte::Matte(const Spectrum &r) : _Kr(r)
    {
    }

    void Matte::ComputeScatteringFunctions(SurfaceInteraction &si, MemoryArena &arena) const
    {
        //每一个Interaction的BSDF都可能不一样，故生命周期由由该Interaction掌管
        si._bsdf = ARENA_ALLOC(arena, BSDF)(si);

        if (!_Kr.isBlack())
        {
            si._bsdf->Add(ARENA_ALLOC(arena, LambertianReflection)(_Kr));
        }
    }
}