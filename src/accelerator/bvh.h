
#ifndef ACCELERATOR_BVH_AGGREGATE_CPP_
#define ACCELERATOR_BVH_AGGREGATE_CPP_

#include <core/primitive.h>
#include <math/bounds.h>
namespace platinum
{ //代表一个图元的部分信息
    struct BVHPrimitiveInfo
    {
        BVHPrimitiveInfo() {}
        BVHPrimitiveInfo(size_t primitiveNumber, const Bounds3f &bounds)
            : primitiveNumber(primitiveNumber),
              bounds(bounds),
              centroid(.5f * bounds._p_min + .5f * bounds._p_max) {}
        size_t primitiveNumber;
        Bounds3f bounds;
        Vector3f centroid;
    };

    // bvh可以理解为一个二叉树
    // 这是构建二叉树时的节点对象
    // 构建完毕之后会转成连续内存的储存方式
    struct BVHBuildNode
    {
        void initLeaf(int first, int n, const Bounds3f &b)
        {
            firstPrimOffset = first;
            nPrimitives = n;
            bounds = b;
            children[0] = children[1] = nullptr;
        }
        void initInterior(int axis, BVHBuildNode *c0, BVHBuildNode *c1)
        {
            children[0] = c0;
            children[1] = c1;
            bounds = UnionBounds(c0->bounds, c1->bounds);
            splitAxis = axis;
            nPrimitives = 0;
        }
        Bounds3f bounds;
        // 二叉树的两个子节点
        BVHBuildNode *children[2];
        // 分割的坐标轴
        int splitAxis;
        // 第一个片元的偏移量
        int firstPrimOffset;
        // 片元数量
        int nPrimitives;
    };

    // 莫顿码片元
    struct MortonPrimitive
    {
        int primitiveIndex;
        uint32_t mortonCode;
    };

    // 用于lbvh
    struct LBVHTreelet
    {
        int startIndex, nPrimitives;
        BVHBuildNode *buildNodes;
    };

    /*
     在内存中的一个线性BVH节点
     布局如下
         A
        / \
       B   C
      / \
     D   E

     线性顺序为 A B D E C
     一个节点的第一个子节点紧接在该节点的后面
     */
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
        uint8_t pad[1];       // 确保64个字节为一个对象，提高缓存命中率
    };

    /*
     根据对象划分
     */
    class BVHAccel : public Aggregate
    {
    public:
        enum SplitMethod
        {
            SAH,
            HLBVH,
            Middle,
            EqualCounts
        };
        BVHAccel(const PropertyTree &node);

        virtual Bounds3f WorldBound() const override;

        virtual ~BVHAccel();

        virtual bool Hit(const Ray &ray) const override;

        virtual bool Hit(const Ray &ray, SurfaceInteraction &inter) const;

        virtual std::string ToString() const { return "BVHAggregate"; }

    private:
        virtual void Initialize() override;

        BVHBuildNode *recursiveBuild(MemoryArena &arena, std::vector<BVHPrimitiveInfo> &primitiveInfo,
                                     int start, int end, int *totalNodes,
                                     std::vector<Ptr<Primitive>> &orderedPrims);

        int flattenBVHTree(BVHBuildNode *node, int *offset);

        int _maxPrimsInNode;

        SplitMethod _splitMethod;

        LinearBVHNode *_nodes = nullptr;
    };

} // namespace platinum

#endif /* bvh_hpp */
