#include <material/mirror.h>

namespace platinum
{

    REGISTER_CLASS(Mirror, "Mirror");

    Mirror::Mirror(const PropertyNode &root)
    {
        std::array<float, 3> spectrum;
        auto iter = root.get_child("R").begin();
        for (size_t i = 0; i < 3; ++i, ++iter)
        {
            spectrum[i] = iter->second.get_value<float>();
        }
        _Kr = Spectrum::fromRGB(spectrum);
        _ref = std::make_shared<SpecularReflection>(_Kr);
    }

    Mirror::Mirror(const Spectrum &r) : _Kr(r)
    {
        _ref = std::make_shared<SpecularReflection>(_Kr);
    }

    void Mirror::ComputeScatteringFunctions(SurfaceInteraction &si) const
    {
        si._bsdf = std::make_shared<BSDF>(si);
        if (!_Kr.isBlack())
        {
            si._bsdf->Add(_ref.get());
        }
    }
}