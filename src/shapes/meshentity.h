#ifndef SHAPES_MESHENTITY_H_
#define SHAPES_MESHENTITY_H_

#include "core/entity.h"

namespace platinum
{
    class MeshEntity : public Entity
    {
    public:
        typedef std::shared_ptr<MeshEntity> ptr;

        MeshEntity(const PropertyTreeNode &node);

        virtual std::string toString() const override { return "MeshEntity[]"; }

    private:
        TriangleMesh::unique_ptr m_mesh;
    };
}

#endif