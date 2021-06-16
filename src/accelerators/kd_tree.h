#ifndef ACCELERATORS_KD_TREE_H_
#define ACCELERATORS_KD_TREE_H_

#include "hitable.h"

namespace platinum
{
    class KdTreeNode;
    class BoundEdge;
    class KdTree : public HitableAggregate
    {
    public:
        typedef std::shared_ptr<KdTree> ptr;

        KdTree(const std::vector<Hitable::ptr> &hitables, int isectCost = 80, int traversalCost = 1,
               Float emptyBonus = 0.5, int maxPrims = 1, int maxDepth = -1);

        virtual Bounds3f worldBound() const override { return m_bounds; }
        ~KdTree();

        virtual bool hit(const Ray &ray) const override;
        virtual bool hit(const Ray &ray, SurfaceInteraction &iset) const override;

        virtual std::string toString() const override { return "KdTree[]"; }

    private:
        void buildTree(int nodeNum, const Bounds3f &bounds,
                       const std::vector<Bounds3f> &primBounds, int *primNums,
                       int nprims, int depth,
                       const std::unique_ptr<BoundEdge[]> edges[3], int *prims0,
                       int *prims1, int badRefines = 0);

        // SAH split measurement
        const Float m_emptyBonus;
        const int m_isectCost, m_traversalCost, m_maxHitables;

        // Compact the node into an array
        KdTreeNode *m_nodes;
        int m_nAllocedNodes, m_nextFreeNode;

        Bounds3f m_bounds;
        std::vector<Hitable::ptr> m_hitables;
        std::vector<int> m_hitableIndices;
    };

    struct KdToDo
    {
        const KdTreeNode *node;
        Float tMin, tMax;
    };
}

#endif