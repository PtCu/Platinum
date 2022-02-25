

#ifndef CORE_TEXTURE_H_
#define CORE_TEXTURE_H_

#include <core/object.h>

namespace platinum
{
    template <typename T>
    class Texture : public Object
    {
    public:
        // Texture Interface
        virtual T Evaluate(const SurfaceInteraction &) const = 0;
        virtual ~Texture() {}
    };
}

#endif