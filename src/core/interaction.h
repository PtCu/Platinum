#ifndef CORE_INTERACTION_H_
#define CORE_INTERACTION_H_

#include "core/platinum.h"
#include "core/primitive.h"

namespace platinum
{
    struct Interaction
    {
        Interaction() : time(0) {}
        Interaction(const Point3f &p, const Normalf &n, const Vector3f &pError, const Vector3f &wo, float time) : p(p), time(time), wo(glm::normalize(wo)), n(n) {}

        Point3f p;
        float time;
        Vector3f wo;
        Normal3f n;
    };

    class SurfaceInteraction : public Interaction
    {
    public:
        SurfaceInteraction() {}
        void ComputeScatteringFunctions();
        const Shape *shape = nullptr;
        const Primitive *primitive = nullptr;
    };
}

#endif