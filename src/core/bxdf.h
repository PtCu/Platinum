#ifndef CORE_BXDF_H_
#define CORE_BXDF_H_

#include "core/platinum.h"

namespace platinum
{
    struct ScatterSamplingRecord
    {
    };

    class BxDF
    {
    public:
        BxDF();
        ~BxDF();
    };
}

#endif