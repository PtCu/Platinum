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

#include <geometry/triangle.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace platinum
{

    void Triangle_::Sample(HitRst &rst, float &pdf) const
    {
        float x = std::sqrt(Random::UniformFloat()), y = Random::UniformFloat();
        rst.record.vert.position_ = A.position_ * (1.0f - x) + B.position_ * (x * (1.0f - y)) + C.position_ * (x * y);
        rst.record.vert.normal_ = this->normal_;
        pdf = 1.0f / area_;
    }
    AABB Triangle_::GetBoundingBox() const
    {
        return bounding_box_;
    }
    float Triangle_::GetArea() const
    {
        return area_;
    }
    Triangle_::Triangle_(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, std::shared_ptr<Material> material_)
        : A(a), B(b), C(c), Object(material_)
    {
        e1 = B.position_ - A.position_;
        e2 = C.position_ - A.position_;
        normal_ = glm::normalize(glm::cross(e1, e2));
        area_ = 0.5f * glm::length(glm::cross(e1, e2));
        bounding_box_ = AABB(A.position_, B.position_);
        bounding_box_.UnionWith(C.position_);
    }
    Triangle_::Triangle_(const Vertex &a, const Vertex &b, const Vertex &c, std::shared_ptr<Material> material_)
        : A(a), B(b), C(c), Object(material_)
    {
        e1 = B.position_ - A.position_;
        e2 = C.position_ - A.position_;
        normal_ = glm::normalize(glm::cross(e1, e2));
        area_ = 0.5f * glm::length(glm::cross(e1, e2));
        bounding_box_ = AABB(A.position_, B.position_);
        bounding_box_.UnionWith(C.position_);
    }
    glm::vec4 Triangle_::intersectRay(const glm::vec3 &o, const glm::vec3 &d)
    {
        glm::mat3 equation_A(glm::vec3(A.position_ - B.position_), glm::vec3(A.position_ - C.position_), d);

        if (glm::abs(glm::determinant(equation_A)) < EPSILON)
            return glm::vec4(0, 0, 0, 0);

        glm::vec3 equation_b = A.position_ - o;
        glm::vec3 equation_X = glm::inverse(equation_A) * equation_b;
        float alpha = 1 - equation_X[0] - equation_X[1];
        return glm::vec4(alpha, equation_X);
    }

    HitRst Triangle_::Intersect(const Ray &r)
    {
        HitRst rst;
        glm::vec4 abgt = this->intersectRay(r.GetOrigin(), r.GetDirection());
        if (abgt == glm::vec4(0) || abgt[0] < 0 || abgt[0] > 1 || abgt[1] < 0 || abgt[1] > 1 || abgt[2] < 0 || abgt[2] > 1 || abgt[3] < r.GetMinTime() || abgt[3] > r.GetMaxTime())
            return HitRst::InValid;

        rst.record.vert = Vertex::GenVert(glm::vec3(abgt[0], abgt[1], abgt[2]), A, B, C);
        rst.record.vert.normal_ = this->normal_;
        rst.record.ray = r;
        rst.material = GetMaterial();
        rst.is_hit = true;
        rst.record.ray.SetTMax(abgt[3]);
        return rst;
    }

    TriangleMesh::TriangleMesh(Transform *object2world, const std::string &filename)
    {
        std::vector<glm::vec3> position_total;
        std::vector<glm::vec3> normal_total;
        std::vector<glm::vec2> uv_total;
        std::vector<int> indices_total;

        auto process_mesh = [&](aiMesh *mesh, const aiScene *scene) -> void
        {
            std::vector<glm::vec3> position;
            std::vector<glm::vec3> normal;
            std::vector<glm::vec2> uv;
            std::vector<int> indices;

            for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
            {
                position.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
                normal.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
                if (mesh->mTextureCoords[0])
                {
                    uv.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
                }
            }

            for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
            {
                aiFace face = mesh->mFaces[i];
                // Retrieve all indices of the face and store them in the indices vector
                for (unsigned int j = 0; j < face.mNumIndices; ++j)
                {
                    indices.push_back(face.mIndices[j] + position_total.size());
                }
            }
            //合并
            position_total.insert(position_total.end(), position.begin(), position.end());
            normal_total.insert(normal_total.end(), normal.begin(), normal.end());
            uv_total.insert(uv_total.end(), uv.begin(), uv.end());
            indices_total.insert(indices_total.end(), indices.begin(), indices.end());
        };

        std::function<void(aiNode * node, const aiScene *scene)> process_node;
        process_node = [&](aiNode *node, const aiScene *scene) -> void
        {
            // Process each mesh located at the current node
            for (unsigned int i = 0; i < node->mNumMeshes; ++i)
            {
                // The node object only contains indices to index the actual objects in the scene.
                // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                process_mesh(mesh, scene);
            }
            for (unsigned int i = 0; i < node->mNumChildren; ++i)
            {
                process_node(node->mChildren[i], scene);
            }
        };

        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                               aiProcess_FlipUVs | aiProcess_FixInfacingNormals | aiProcess_OptimizeMeshes);

        //处理节点
        process_node(scene->mRootNode, scene);

        _num_vertices = position_total.size();
        _position.reset(new glm::vec3[_num_vertices]);
        if (!normal_total.empty())
        {
            _normal.reset(new glm::vec3[_num_vertices]);
        }
        if (!uv_total.empty())
        {
            _uv.reset(new glm::vec2[_num_vertices]);
        }

        for (unsigned int i = 0; i < _num_vertices; ++i)
        {
            _position[i] = object2world->ExecOn(position_total[i], 1.f);
            if (_normal != nullptr)
            {
                _normal[i] = object2world->ExecOn(normal_total[i], 0.f);
            }
            if (_uv != nullptr)
            {
                _uv[i] = uv_total[i];
            }
        }
        _indices.resize(indices_total.size());
        _indices.assign(indices_total.begin(), indices_total.end());
    }
}
