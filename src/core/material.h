#ifndef CORE_MATERIAL_H_
#define CORE_MATERIAL_H_

#include "platinum.h"

PLATINUM_BEGIN

class Material : public Object
{
public:
    typedef std::shared_ptr<Material> ptr;

    Material() = default;
    virtual ~Material() = default;

    virtual void computeScatteringFunctions(SurfaceInteraction &si, MemoryArena &arena,
                                            TransportMode mode, bool allowMultipleLobes) const = 0;

    virtual ClassType getClassType() const override { return ClassType::AEMaterial; }
};

PLATINUM_END

#endif