

#ifndef CORE_MATERIAL_H_
#define CORE_MATERIAL_H_

#include <core/interaction.h>
#include <core/object.h>

namespace platinum
{
    class Material : public Object
    {
    public:
        Material() = default;

        virtual ~Material() = default;

        virtual void ComputeScatteringFunctions(SurfaceInteraction &si, MemoryArena &arena) const = 0;
    };

} // namespace platinum

#endif
