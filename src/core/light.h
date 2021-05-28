#ifndef CORE_LIGHT_H_
#define CORE_LIGHT_H_

#include "core/platinum.h"
#include "core/primitive.h"

namespace platinum
{
    enum class LightFlags
    {
        // 点光源
        DeltaPosition = 1,
        // 方向光，只有一个方向
        DeltaDirection = 2,
        // 面光源
        Area = 4,
        Infinite = 8,
        // 环境光
        Env = 16
    };

    class Light : public Object
    {
    };
}

#endif