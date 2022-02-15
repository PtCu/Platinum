
#ifndef MATERIAL_MIRROR_H_
#define MATERIAL_MIRROR_H_

#include <material/bxdf/specular.h>
#include <core/material.h>
#include <core/bsdf.h>
#include <core/spectrum.h>

namespace platinum
{
    class Mirror final : public Material
    {
    public:
        Mirror(const PropertyTree &root);

        Mirror(const Spectrum &r);

        virtual void ComputeScatteringFunctions(SurfaceInteraction &si) const override;

        virtual std::string ToString() const override { return "Mirror"; }

    private:
        Spectrum _Kr;
        Ptr<SpecularReflection> _ref;
    };
}

#endif