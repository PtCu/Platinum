#ifndef CORE_AGGREGATE_H_
#define CORE_AGGREGATE_H_

#include "core/platinum.h"
#include "primitive.h"

namespace platinum
{
    class Aggregate : public Primitive
    {
    public:
        void ComputeScatteringFunctions() const {}
    };
}

#endif