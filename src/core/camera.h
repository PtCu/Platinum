#ifndef CORE_CAMERA_H_
#define CORE_CAMERA_H_

#include "core/platinum.h"
#include "core/primitive.h"

namespace platinum
{
    struct CameraSample
    {
        /* data */
    };
    
    class Camera : public Object
    {
    public:
        Camera();
        ~Camera();
    };
}

#endif