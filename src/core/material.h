#ifndef CORE_MATERIAL_H_
#define CORE_MATERIAL_H_

#include "platinum.h"
#include "spectrum.h"
#include "math_utils.h"
#include "rtti.h"

namespace platinum
{

    class Material : public Object
    {
    public:
        typedef std::shared_ptr<Material> ptr;

        Material() = default;
        virtual ~Material() = default;

        virtual void computeScatteringFunctions(SurfaceInteraction &si, MemoryArena &arena,
                                                TransportMode mode, bool allowMultipleLobes) const = 0;

        virtual ClassType getClassType() const override { return ClassType::Material; }
    };

}

#endif