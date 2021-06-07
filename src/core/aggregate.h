#ifndef CORE_AGGREGATE_H_
#define CORE_AGGREGATE_H_

#include "platinum.h"
#include "primitive.h"

PLATINUM_BEGIN

class Aggregate : public Primitive
{
public:
    void ComputeScatteringFunctions() const {}
};

PLATINUM_END

#endif