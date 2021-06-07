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

struct LinearBVHNode
{
    Bounds3f bounds;
    union
    {
        int primitivesOffset;  //指向图元
        int secondChildOffset; // 第二个子节点在数组中的偏移量
    };
    uint16_t nPrimitives; // 图元数量
    uint8_t axis;         // interior node: xyz
    uint8_t pad[1];       // 确保32个字节为一个对象，提高缓存命中率
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
    BVHAccel(std::vector<std::shared_ptr<const Shape>> &shapes, int maxPrimsInNode = 1, SplitMethod splitMethod = SplitMethod::SAH);
    virtual Bounds3f WorldBound() const;
    virtual nloJson toJson() const override
    {
        return nloJson();
    }
    virtual ~BVHAccel();
    //
    virtual bool Intersect(const Ray &ray, SurfaceInteraction *isect) const;
    virtual bool IntersectP(const Ray &ray) const;

private:
    BVHBuildNode *recursiveBuild(std::vector<BVHPrimitiveInfo> &primitiveInfo, int start, int end, int *totalNodes, std::vector<std::shared_ptr<Primitive>> &orderedPrims);
    int flattenBVHTree(BVHBuildNode *node, int *offset);
    const int maxPrimsInNode;
    const SplitMethod splitMethod;
    std::vector<std::shared_ptr<const Shape>> shapes;
    std::vector<const Primitive *> primitives;
    LinearBVHNode *nodes = nullptr;
};
std::shared_ptr<BVHAccel> createBVH(const nloJson &param, const std::vector<std::shared_ptr<const Shape>> &shapes);
#endif