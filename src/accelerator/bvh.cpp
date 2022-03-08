
#include <accelerator/bvh.h>
#include <core/memory.h>
#include <core/timer.h>
#include <tbb/parallel_for.h>
#include <tbb/spin_mutex.h>;
namespace platinum
{

    REGISTER_CLASS(BVHAccel, "BVH");

    BVHAccel::BVHAccel(const PropertyTree &node) : Aggregate(node)
    {
        int maxPrimsInNode = node.Get<int>("MaxPrimsInNode", 1);

        BVHAccel::SplitMethod splitMethod;
        std::string sm = node.Get<std::string>("SplitMethod", "SAH");
        if (sm == "SAH")
        {
            splitMethod = BVHAccel::SplitMethod::SAH;
        }
        else if (sm == "Middle")
        {
            splitMethod = BVHAccel::SplitMethod::Middle;
        }
        else if (sm == "EqualCounts")
        {
            splitMethod = BVHAccel::SplitMethod::EqualCounts;
        }
    }

    void BVHAccel::Initialize()
    {
        LOG(INFO) << "Construct BVH accelerator..";
        Timer timer("BVH initialize");
        std::vector<BVHPrimitiveInfo> _primitiveInfo(_primitives.size());

        //并行构建
        tbb::spin_mutex mtx;
        tbb::parallel_for(tbb::blocked_range<size_t>(0, _primitives.size()),
                          [&](tbb::blocked_range<size_t> r)
                          {
                              Bounds3f local_bound;
                              for (size_t i = r.begin(); i < r.end(); ++i)
                              {
                                  auto aabb = _primitives[i]->WorldBound();
                                  local_bound = UnionBounds(local_bound, aabb);
                                  // 储存每个aabb的中心以及索引
                                  _primitiveInfo[i] = {i, aabb};
                              }
                              std::lock_guard lck(mtx);
                              _world_bounds = UnionBounds(_world_bounds, local_bound);
                          });

        MemoryArena arena(1024 * 1024);
        int totalNodes = 0;

        std::vector<Ptr<Primitive>> orderedPrims;
        BVHBuildNode *root;

        if (_splitMethod == SplitMethod::HLBVH)
        {
            // 暂时不实现
            DCHECK(false);
        }
        else
        {
            root = recursiveBuild(arena, _primitiveInfo, 0, _primitiveInfo.size(), &totalNodes, orderedPrims);
        }

        _primitives.swap(orderedPrims);
        _primitiveInfo.resize(0);

        _nodes = AllocAligned<LinearBVHNode>(totalNodes);
        int Offset = 0;
        // 将二叉树结构的bvh转换成连续储存结构
        flattenBVHTree(root, &Offset);
        CHECK_EQ(totalNodes, Offset);
    }

    Bounds3f BVHAccel::WorldBound() const
    {
        return _nodes ? _nodes[0].bounds : Bounds3f();
    }

    BVHAccel::~BVHAccel()
    {
    }

    /**
     * 基本思路
     * 根据根据光线的方向以及当前节点的分割轴
     * 选择较近的一个子节点求交，远的子节点放入栈中
     * 近的子节点如果与光线没有交点，则对栈中的节点求交
     * 循环以上过程
     */
    bool BVHAccel::Hit(const Ray &ray) const
    {
        if (!_nodes)
        {
            return false;
        }
        Vector3f invDir(1.f / ray._direction.x, 1.f / ray._direction.y, 1.f / ray._direction.z);
        int dirIsNeg[3] = {invDir.x < 0, invDir.y < 0, invDir.z < 0};

        // 即将访问的节点，栈结构
        int nodesToVisit[64];

        // 即将要访问到的节点
        int toVisitOffset = 0;

        // 储存在包围盒在_node数组中节点的位置
        int currentNodeIndex = 0;

        // 从根节点开始遍历
        // 从根节点开始遍历
        while (true)
        {
            const LinearBVHNode *node = &_nodes[currentNodeIndex];
            if (node->bounds.Hit(ray, invDir, dirIsNeg))
            {

                if (node->nPrimitives > 0)
                {
                    // 叶子节点
                    for (int i = 0; i < node->nPrimitives; ++i)
                    {
                        // 逐个片元判断求交
                        if (_primitives[node->primitivesOffset + i]->Hit(ray))
                        {
                            return true;
                        }
                    }
                    if (toVisitOffset == 0)
                    {
                        break;
                    }

                    // 取出栈中的节点求交
                    currentNodeIndex = nodesToVisit[--toVisitOffset];
                }
                else
                {
                    // 内部节点
                    if (dirIsNeg[node->axis])
                    {
                        // 如果ray的方向为负，则先判断右子树，把左子树压入栈中
                        // 下次循环时直接判断与右子树是否有相交，如果没有相交
                        // 则访问栈中的节点求交
                        nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
                        currentNodeIndex = node->secondChildOffset;
                    }
                    else
                    {
                        // 如果ray的方向为正，则先判断左子树，把右子树压入栈中
                        nodesToVisit[toVisitOffset++] = node->secondChildOffset;
                        currentNodeIndex = currentNodeIndex + 1;
                    }
                }
            }
            else
            {

                //如果没有相交 则访问栈中的节点求交
                if (toVisitOffset == 0)
                {
                    break;
                }
                currentNodeIndex = nodesToVisit[--toVisitOffset];
            }
        }
        return false;
    }

    BVHBuildNode *BVHAccel::recursiveBuild(MemoryArena &arena, std::vector<BVHPrimitiveInfo> &primitiveInfo, int start, int end, int *totalNodes, std::vector<Ptr<Primitive>> &orderedPrims)
    {
        BVHBuildNode *node = ARENA_ALLOC(arena, BVHBuildNode);
        Bounds3f bounds;
        for (int i = start; i < end; ++i)
        {
            bounds = UnionBounds(bounds, primitiveInfo[i].bounds);
        }
        (*totalNodes)++;
        int numPrimitives = end - start;
        if (numPrimitives == 1)
        {
            // 生成叶子节点
            int firstPrimOffset = orderedPrims.size();
            int primNum = primitiveInfo[start].primitiveNumber;
            orderedPrims.push_back(_primitives[primNum]);
            node->initLeaf(firstPrimOffset, numPrimitives, bounds);
            return node;
        }
        else
        {
            Bounds3f centroidBounds;
            for (int i = start; i < end; ++i)
            {
                centroidBounds = UnionBounds(centroidBounds, primitiveInfo[i].centroid);
            }
            // 范围最广的维度
            int maxDim = centroidBounds.MaximumExtent();
            int mid = (start + end) / 2;
            // 如果最centroidBounds为一个点，则初始化叶子节点
            if (centroidBounds._p_max[maxDim] == centroidBounds._p_min[maxDim])
            {
                int firstPrimOffset = orderedPrims.size();
                for (int i = start; i < end; ++i)
                {
                    int primNum = primitiveInfo[i].primitiveNumber;
                    orderedPrims.push_back(_primitives[primNum]);
                }
                node->initLeaf(firstPrimOffset, numPrimitives, bounds);
                return node;
            }
            else
            {
                switch (_splitMethod)
                {
                case Middle:
                {
                    // 选择范围最大的维度的中点进行划分
                    float pmid = (centroidBounds._p_min[maxDim] + centroidBounds._p_max[maxDim]) / 2;
                    auto func = [maxDim, pmid](const BVHPrimitiveInfo &pi)
                    {
                        return pi.centroid[maxDim] < pmid;
                    };
                    // std::partition会将区间[first,last)中的元素重新排列，
                    //满足判断条件的元素会被放在区间的前段，不满足的元素会被放在区间的后段。
                    // 返回中间指针
                    BVHPrimitiveInfo *midPtr = std::partition(&primitiveInfo[start], &primitiveInfo[end - 1] + 1, func);
                    mid = midPtr - &primitiveInfo[0];
                    if (mid != start && mid != end)
                    {
                        break;
                    }
                }

                case EqualCounts:
                {
                    // 相等数量划分
                    mid = (start + end) / 2;
                    auto func = [maxDim](const BVHPrimitiveInfo &a,
                                         const BVHPrimitiveInfo &b)
                    {
                        return a.centroid[maxDim] < b.centroid[maxDim];
                    };
                    //将迭代器指向的从_First 到 _last 之间的元素进行二分排序，
                    //以_Nth 为分界，前面都比 _Nth 小（大），后面都比之大（小）；
                    //但是两段内并不有序。
                    std::nth_element(&primitiveInfo[start],
                                     &primitiveInfo[mid],
                                     &primitiveInfo[end - 1] + 1,
                                     func);
                    break;
                }

                case SAH:
                {
                    // 表面启发式划分
                    // 假设每个片元的求交耗时都相等
                    // 求交消耗的时间为 C = ∑[t=i,N]t(i)
                    // N为片元的数量，t(i)为第i个片元求交耗时
                    // C(A,B) = t1 + p(A) * C(A) + p(B) * C(B)
                    // t1为遍历内部节点所需要的时间加上确定光线通过哪个子节点的时间
                    // p为概率，C为求交耗时
                    // 概率与表面积成正比
                    if (numPrimitives <= 2)
                    {
                        mid = (start + end) / 2;
                        auto func = [maxDim](const BVHPrimitiveInfo &a,
                                             const BVHPrimitiveInfo &b)
                        {
                            return a.centroid[maxDim] < b.centroid[maxDim];
                        };
                        std::nth_element(&primitiveInfo[start],
                                         &primitiveInfo[mid],
                                         &primitiveInfo[end - 1] + 1,
                                         func);
                    }
                    else
                    {
                        // 在范围最广的维度上等距离添加n-1个平面
                        // 把空间分为n个部分，可以理解为n个桶
                        struct BucketInfo
                        {
                            int count = 0;
                            Bounds3f bounds;
                        };
                        // 默认12个桶
                        constexpr int nBuckets = 12;
                        BucketInfo buckets[nBuckets];
                        // 统计每个桶中的bounds以及片元数量
                        for (int i = start; i < end; ++i)
                        {
                            int b = nBuckets * centroidBounds.Offset(primitiveInfo[i].centroid)[maxDim];
                            if (b == nBuckets)
                            {
                                b = nBuckets - 1;
                            }
                            buckets[b].count++;
                            buckets[b].bounds = UnionBounds(buckets[b].bounds, primitiveInfo[i].bounds);
                        }

                        // 找出最优的分割方式，目前假设12个桶，则分割方式有11种
                        // 1与11，2与10，3与9，等等11个组合，估计出每个组合的计算耗时
                        // 从而找出最优的分割方式
                        float cost[nBuckets - 1];
                        for (int i = 0; i < nBuckets - 1; ++i)
                        {
                            Bounds3f b0, b1;
                            int count0 = 0, count1 = 0;
                            // 计算第一部分
                            for (int j = 0; j <= i; ++j)
                            {
                                b0 = UnionBounds(b0, buckets[j].bounds);
                                count0 += buckets[j].count;
                            }
                            // 计算第二部分
                            for (int j = i + 1; j < nBuckets; ++j)
                            {
                                b1 = UnionBounds(b1, buckets[j].bounds);
                                count1 += buckets[j].count;
                            }

                            // 参见公式  C(A,B) = t1 + p(A) * C(A) + p(B) * C(B)
                            // p(A) = S(A) / S, p(B) = S(B) / S
                            // 概率与表面积成正比，耗时与片元个数成，
                            // 假设C(A) = count(A)
                            // 则可以写成以下形式

                            // 第一部分的总面积
                            float s0 = count0 * b0.SurfaceArea();
                            // 第二部分的总面积
                            float s1 = count1 * b1.SurfaceArea();
                            // pbrt最新代码把0.125改成了1
                            cost[i] = 1 + (s0 + s1) / bounds.SurfaceArea();
                        }

                        // 找到最小耗时的分割方式
                        float minCost = cost[0];
                        int minCostSplitBucket = 0;
                        for (int i = 1; i < nBuckets - 1; ++i)
                        {
                            if (cost[i] < minCost)
                            {
                                minCost = cost[i];
                                minCostSplitBucket = i;
                            }
                        }
                        // 假设叶子节点的求交耗时等于片元个数
                        float leafCost = numPrimitives;
                        auto func = [=](const BVHPrimitiveInfo &pi)
                        {
                            int b = nBuckets * centroidBounds.Offset(pi.centroid)[maxDim];
                            if (b == nBuckets)
                            {
                                b = nBuckets - 1;
                            }
                            CHECK_GE(b, 0);
                            CHECK_LT(b, nBuckets);
                            return b <= minCostSplitBucket;
                        };

                        if (numPrimitives > _maxPrimsInNode || minCost < leafCost)
                        {
                            // pmid指向第一个func为false的元素的
                            BVHPrimitiveInfo *pmid = std::partition(&primitiveInfo[start],
                                                                    &primitiveInfo[end - 1] + 1,
                                                                    func);
                            mid = pmid - &primitiveInfo[0];
                        }
                        else
                        {
                            // 创建叶子节点
                            int firstPrimOffset = orderedPrims.size();
                            for (int i = start; i < end; ++i)
                            {
                                int primNum = primitiveInfo[i].primitiveNumber;
                                orderedPrims.push_back(_primitives[primNum]);
                            }
                            node->initLeaf(firstPrimOffset, numPrimitives, bounds);
                            return node;
                        }
                    }
                }
                default:
                    break;
                }
                node->initInterior(maxDim,
                                   recursiveBuild(arena, primitiveInfo, start, mid,
                                                  totalNodes, orderedPrims),
                                   recursiveBuild(arena, primitiveInfo, mid, end,
                                                  totalNodes, orderedPrims));
            }
        }
        return node;
    }

    bool BVHAccel::Hit(const Ray &ray, SurfaceInteraction &isect) const
    {
        if (!_nodes)
        {
            return false;
        }

        bool hit = false;
        Vector3f invDir(1 / ray._direction.x, 1 / ray._direction.y, 1 / ray._direction.z);
        int dirIsNeg[3] = {invDir.x < 0, invDir.y < 0, invDir.z < 0};

        int toVisitOffset = 0, currentNodeIndex = 0;
        int nodesToVisit[64];
        while (true)
        {
            const LinearBVHNode *node = &_nodes[currentNodeIndex];

            if (node->bounds.Hit(ray, invDir, dirIsNeg))
            {
                if (node->nPrimitives > 0)
                {
                    // 叶子节点
                    for (int i = 0; i < node->nPrimitives; ++i)
                    {
                        // 逐个片元判断求交
                        if (_primitives[node->primitivesOffset + i]->Hit(ray, isect))
                        {
                            hit = true;
                        }
                    }
                    if (toVisitOffset == 0)
                    {
                        break;
                    }
                    // 取出栈中的节点求交
                    currentNodeIndex = nodesToVisit[--toVisitOffset];
                    if (currentNodeIndex == 1)
                    {
                    }
                }
                else
                {
                    // 内部节点
                    if (dirIsNeg[node->axis])
                    {
                        // 如果ray的方向为负，则先判断右子树，把左子树压入栈中
                        // 下次循环时直接判断与右子树是否有相交，如果没有相交
                        // 则访问栈中的节点求交
                        nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
                        currentNodeIndex = node->secondChildOffset;
                    }
                    else
                    {
                        // 如果ray的方向为正，则先判断左子树，把右子树压入栈中
                        nodesToVisit[toVisitOffset++] = node->secondChildOffset;
                        currentNodeIndex = currentNodeIndex + 1;
                    }
                }
            }
            else
            {
                //如果没有相交 则访问栈中的节点求交
                if (toVisitOffset == 0)
                {
                    break;
                }
                currentNodeIndex = nodesToVisit[--toVisitOffset];
            }
        }
        return hit;
    }

    int BVHAccel::flattenBVHTree(BVHBuildNode *node, int *Offset)
    {
        LinearBVHNode *linearNode = &_nodes[*Offset];
        linearNode->bounds = node->bounds;
        int myOffset = (*Offset)++;
        if (node->nPrimitives > 0)
        {
            // 初始化叶子节点
            DCHECK(!node->children[0] && !node->children[1]);
            CHECK_LT(node->nPrimitives, 65536);
            linearNode->primitivesOffset = node->firstPrimOffset;
            linearNode->nPrimitives = node->nPrimitives;
        }
        else
        {
            // 初始化内部节点
            linearNode->axis = node->splitAxis;
            linearNode->nPrimitives = 0;
            flattenBVHTree(node->children[0], Offset);
            linearNode->secondChildOffset =
                flattenBVHTree(node->children[1], Offset);
        }
        return myOffset;
    }

    //"param" : {
    //    "maxPrimsInNode" : 1,
    //    "splitMethod" : "SAH"
    //}

}
