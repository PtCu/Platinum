#ifndef CORE_SHAPE_H_
#define CORE_SHAPE_H_

#include "core/platinum.h"
#include "core/primitive.h"

namespace platinum
{
    enum ShapeType
    {
        EMesh,
        ESphere,
        EDisk,
        ECone,
        ECylinder
    };

    class Shape : public Primitive
    {
    public:
        Shape();
        virtual ~Shape();
    };

#endif