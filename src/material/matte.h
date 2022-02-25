

#ifndef MATERIAL_MATTE_H_
#define MATERIAL_MATTE_H_

#include <core/material.h>
#include <material/bxdf/lambertian.h>
#include <core/bsdf.h>
#include <core/spectrum.h>
namespace platinum
{
    class Matte final : public Material
    {
    public:
        Matte();

        Matte(const Spectrum &r);

        Matte(const PropertyTree &node);

        virtual void ComputeScatteringFunctions(SurfaceInteraction &si, MemoryArena &arena) const override;

        virtual std::string ToString() const { return "Matte"; }

    private:
        Spectrum _Kr;
    };
}

#endif