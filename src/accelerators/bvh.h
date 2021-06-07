#ifndef ACCELERATORS_BVH_H_
#define ACCELERATORS_BVH_H_

#include "core/aggregate.h"
#include "core/platinum.h"

struct BVHBuildNode
{
    void InitLeaf(int first, int n, const Bounds3f &b);
    void InitInterior(int axis, BVHBuildNode *c0, BVHBuildNode *cl);
    Bounds3f bounds;
    BVHBuildNode *children[2];
    int splitAxis, firstPrimsOffset, aPrimitives;
};

struct LinearBVHNode{
    Bounds3f bounds;
    union{
        int primitivesOffset;
        int secondChildOffset;
    };
    
}

class BVHAccel : public Aggregate
{

public:
    enum class SplitMethod
    {
        SAH,
        HLBVH,
        Middle,
        EqualCounts
    };
    BVHAccel(std::vector<std::shared_ptr<Primitive>> p, int maxPrimsInNode = 1, SplitMethod splitMethod = SplitMethod::SAH);
    Bounds3f WorldBound() const;
    ~BVHAccel();
    bool Intersect(const Ray &ray, SurfaceInteraction *isect) const;
    bool IntersectP(const Ray &ray) const;

private:
    BVHBuildNode *recursiveBuild(std::vector<BVHPrimitiveInfo> &primitiveInfo, int start, int end, int *totalNodes, std::vector<std::shared_ptr<Primitive>> &orderedPrims);
    int flattenBVHTree(BVHBuildNode *node, int *offset);
    const int maxPrimsInNode;
    const SplitMethod splitMethod;
    std::vector<std::shared_ptr<Primitive>> primitives;
    LinearBVHNode *nodes = nullptr;
};

#endif