#include <material/mirror.h>

namespace platinum
{

    REGISTER_CLASS(Mirror, "Mirror");

    Mirror::Mirror(const PropertyTree &root)
    {
        Vector3f spectrum = root.Get<Vector3f>("R");
        _Kr = Spectrum::fromRGB(spectrum);
        auto fre = std::make_shared<FresnelNoOp>();
    }

    Mirror::Mirror(const Spectrum &r) : _Kr(r)
    {
        auto fre = std::make_shared<FresnelNoOp>();
    }

    void Mirror::ComputeScatteringFunctions(SurfaceInteraction &si, MemoryArena &arena) const
    {
        si._bsdf = ARENA_ALLOC(arena, BSDF)(si);
        if (!_Kr.isBlack())
        {
            si._bsdf->Add(ARENA_ALLOC(arena, SpecularReflection)(
                _Kr, ARENA_ALLOC(arena, FresnelNoOp)()));
        }
    }
}