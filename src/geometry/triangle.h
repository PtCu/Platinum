// The MIT License (MIT)

// Copyright (c) 2021 PtCu

//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

#ifndef GEOMETRY_TRIANGLE_H_
#define GEOMETRY_TRIANGLE_H_

#include <core/object.h>
#include <geometry/vertex.h>
#include <core/shape.h>

namespace platinum
{
    class Triangle_ : public Object
    {
    public:
        Triangle_(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, std::shared_ptr<Material> material_ = NULL);
        Triangle_(const Vertex& a, const Vertex& b, const Vertex& c, std::shared_ptr<Material> material_ = NULL);
        virtual HitRst Intersect(const Ray& r);
        virtual AABB GetBoundingBox() const;
        virtual float GetArea() const;
        virtual void Sample(HitRst& inter, float& pdf) const;
        const Vertex& GetA() const { return A; }
        const Vertex& GetB() const { return B; }
        const Vertex& GetC() const { return C; }

    protected:
        glm::vec4 intersectRay(const glm::vec3& o, const glm::vec3& d);

    private:
        Vertex A, B, C;
        glm::vec3 normal_, e1, e2;
        float area_{ 0.f };
        AABB bounding_box_;
    };

    class TriangleMesh final
    {
    public:
        TriangleMesh(Transform* object2world, const std::string& filename);
        const glm::vec3& GetPositionAt(int index) const { return _position[index]; }
        const glm::vec3& GetNormalAt(int index) const { return _normal[index]; }
        const glm::vec2& GetUVAt(int index) const { return _uv[index]; }
        const std::vector<int>& GetIndices() const { return _indices; }

        bool HasUV() const { return _uv != nullptr; }
        bool HasNormal() const { return _normal != nullptr; }
    private:
        std::unique_ptr<glm::vec3[]> _position{ nullptr };
        std::unique_ptr<glm::vec3[]> _normal{ nullptr };
        std::unique_ptr<glm::vec2[]> _uv{ nullptr };
        std::vector<int> _indices;
        int _num_vertices;
    };

    class Triangle final : public Shape
    {
    public:
        Triangle(Transform* object2world, Transform* wordl2object, std::array<int, 3> indices, TriangleMesh* mesh)
            : Shape(object2world, wordl2object), _indices(indices), _mesh(mesh) {}
        virtual ~Triangle() = default;
        virtual float Area() const override;
        virtual Interaction Sample(const glm::vec2& u, float& pdf) const override;

        virtual AABB ObjectBound() const override;
        virtual AABB WorldBound() const override;

        virtual bool Hit(const Ray& ray) const override;
        virtual bool Hit(const Ray& ray, float& tHit, SurfaceInteraction& isect) const override;

        virtual float SolidAngle(const glm::vec3& p, int nSamples = 512) const override;

    private:
        TriangleMesh* _mesh;
        std::array<int, 3> _indices;
    };
}

#endif
