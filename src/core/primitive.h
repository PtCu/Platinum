#ifndef CORE_PRIMITIVE_H_
#define CORE_PRIMITIVE_H_

#include "core/platinum.h"
#include "math/bound.h"

namespace Platinum
{
    class Object
    {
    public:
    virtual nlmJson toJson() const{
        return nlmJson();
    }
        virtual virtual ~Object() {}
    };

    class Primitive : public Object
    {

      
    };

} // namespace Platinum

#endif