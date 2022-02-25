

#ifndef GEOMETRY_TRIANGLE_H_
#define GEOMETRY_TRIANGLE_H_

#include <core/primitive.h>
#include <core/shape.h>

namespace platinum
{

    class TriangleMesh final
    {
    public:
        TriangleMesh(Transform *object2world, const std::string &filename);
        const Vector3f &GetPositionAt(int index) const { return _position[index]; }
        const Vector3f &GetNormalAt(int index) const { return _normal[index]; }
        const Vector2f &GetUVAt(int index) const { return _uv[index]; }
        const std::vector<int> &GetIndices() const { return _indices; }

        bool HasUV() const { return _uv != nullptr; }
        bool HasNormal() const { return _normal != nullptr; }

    private:
        std::unique_ptr<Vector3f[]> _position{nullptr};
        std::unique_ptr<Vector3f[]> _normal{nullptr};
        std::unique_ptr<Vector2f[]> _uv{nullptr};
        std::vector<int> _indices;
        int _num_vertices;
    };

    class Triangle final : public Shape
    {
    public:
        Triangle(Transform *object2world, Transform *wordl2object, std::array<int, 3> indices, TriangleMesh *mesh)
            : Shape(object2world, wordl2object), _indices(indices), _mesh(mesh) {}
        virtual ~Triangle() = default;
        virtual float Area() const override;
        virtual Interaction Sample(const Vector2f &u, float &pdf) const override;

        virtual Bounds3f ObjectBound() const override;
        virtual Bounds3f WorldBound() const override;

        virtual bool Hit(const Ray &ray) const override;
        virtual bool Hit(const Ray &ray, float &tHit, SurfaceInteraction &isect) const override;

        virtual float SolidAngle(const Vector3f &p, int nSamples = 512) const override;

        virtual std::string ToString() const { return "Triangle"; }

    private:
        TriangleMesh *_mesh;
        std::array<int, 3> _indices;
    };
}

#endif
