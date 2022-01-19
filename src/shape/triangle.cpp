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

#include <shape/triangle.h>
#include <core/sampler.h>
#include <core/interaction.h>
#include <math/transform.h>
#include <glm/gtx/norm.hpp>
#include <math/bounds.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace platinum
{

    TriangleMesh::TriangleMesh(Transform* object2world, const std::string& filename)
    {
        std::vector<glm::vec3> position_total;
        std::vector<glm::vec3> normal_total;
        std::vector<glm::vec2> uv_total;
        std::vector<int> indices_total;

        auto process_mesh = [&](aiMesh* mesh, const aiScene* scene) -> void
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

        std::function<void(aiNode* node, const aiScene* scene)> process_node;
        process_node = [&](aiNode* node, const aiScene* scene) -> void
        {
            // Process each mesh located at the current node
            for (unsigned int i = 0; i < node->mNumMeshes; ++i)
            {
                // The node object only contains indices to index the actual objects in the scene.
                // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                process_mesh(mesh, scene);
            }
            for (unsigned int i = 0; i < node->mNumChildren; ++i)
            {
                process_node(node->mChildren[i], scene);
            }
        };

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs | aiProcess_FixInfacingNormals | aiProcess_OptimizeMeshes);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            LOG(FATAL) << "ERROR::ASSIMP" << importer.GetErrorString();
        }

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

    Bounds3f Triangle::ObjectBound() const
    {
        const auto& p0 = _mesh->GetPositionAt(_indices[0]);
        const auto& p1 = _mesh->GetPositionAt(_indices[1]);
        const auto& p2 = _mesh->GetPositionAt(_indices[2]);
        return UnionBounds(Bounds3f(_world2object->ExecOn(p0, 1.f), _world2object->ExecOn(p1, 1.f)), _world2object->ExecOn(p2, 1.f));
    }

    Bounds3f Triangle::WorldBound() const
    {
        const auto& p0 = _mesh->GetPositionAt(_indices[0]);
        const auto& p1 = _mesh->GetPositionAt(_indices[1]);
        const auto& p2 = _mesh->GetPositionAt(_indices[2]);
        return UnionBounds(Bounds3f(p0, p1), p2);
    }
    float Triangle::Area()const {
        const auto& p0 = _mesh->GetPositionAt(_indices[0]);
        const auto& p1 = _mesh->GetPositionAt(_indices[1]);
        const auto& p2 = _mesh->GetPositionAt(_indices[2]);
        return 0.5f * glm::length(glm::cross(p1 - p0, p2 - p0));
    }

    Interaction Triangle::Sample(const glm::vec2& u, float& pdf) const
    {
        //重心坐标
        glm::vec2 b = UniformSampleTriangle(u);
        // Get triangle vertices in _p0_, _p1_, and _p2_
        const auto& p0 = _mesh->GetPositionAt(_indices[0]);
        const auto& p1 = _mesh->GetPositionAt(_indices[1]);
        const auto& p2 = _mesh->GetPositionAt(_indices[2]);
        Interaction it;
        it.p = b[0] * p0 + b[1] * p1 + (1 - b[0] - b[1]) * p2;
        // Compute surface normal for sampled point on triangle
        it.n = glm::normalize(glm::vec3(glm::cross(p1 - p0, p2 - p0)));

        pdf = 1.f / Area();
        return it;
    }

    bool Triangle::Hit(const Ray& ray) const {
        const auto& p0 = _mesh->GetPositionAt(_indices[0]);
        const auto& p1 = _mesh->GetPositionAt(_indices[1]);
        const auto& p2 = _mesh->GetPositionAt(_indices[2]);


        glm::vec3 p0t = p0 - glm::vec3(ray._origin);
        glm::vec3 p1t = p1 - glm::vec3(ray._origin);
        glm::vec3 p2t = p2 - glm::vec3(ray._origin);
        int kz = maxDimension(glm::abs(ray._direction));
        // int kx = kz + 1;
        // if (kx == 3) kx = 0;
        // int ky = kx + 1;
        // if (ky == 3) ky = 0;
        int kx = (kz + 1) % 3;
        int ky = (kx + 1) % 3;
        glm::vec3 d = permute(ray._direction, kx, ky, kz);
        p0t = permute(p0t, kx, ky, kz);
        p1t = permute(p2t, kx, ky, kz);
        p2t = permute(p1t, kx, ky, kz);

        float Sx = -d.x / d.z;
        float Sy = -d.y / d.z;
        float Sz = 1.f / d.z;
        p0t.x += Sx * p0t.z;
        p0t.y += Sy * p0t.z;
        p1t.x += Sx * p1t.z;
        p1t.y += Sy * p1t.z;
        p2t.x += Sx * p2t.z;
        p2t.y += Sy * p2t.z;

        //原点分别和三个边求叉乘
        float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
        float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
        float e2 = p0t.x * p1t.y - p0t.y * p1t.x;

        // 不同号时，原点不在三角形内
        if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
            return false;
        //和为0时，原点不在三角形内，在三角形边上
        float det = e0 + e1 + e2;
        if (det == 0)
            return false;

        //由于不相交的情况远多于相交的情况，所以先再次排除一些不相交的情况，以避免重复算浮点数除法
        p0t.z *= Sz;
        p1t.z *= Sz;
        p2t.z *= Sz;
        float t_scaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
        if (det < 0 && (t_scaled >= 0 || t_scaled < ray._t_max * det))
            return false;
        else if (det > 0 && (t_scaled <= 0 || t_scaled > ray._t_max * det))
            return false;

        //用小三角形面积算重心坐标
        float invDet = 1 / det;
        float b0 = e0 * invDet;
        float b1 = e1 * invDet;
        float b2 = e2 * invDet;
        float t = t_scaled * invDet;

        // Compute $\delta_z$ term for triangle $t$ error bounds
        float maxZt = maxComponent(glm::abs(glm::vec3(p0t.z, p1t.z, p2t.z)));
        float deltaZ = gamma(3) * maxZt;

        // Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
        float maxXt = maxComponent(glm::abs(glm::vec3(p0t.x, p1t.x, p2t.x)));
        float maxYt = maxComponent(glm::abs(glm::vec3(p0t.y, p1t.y, p2t.y)));
        float deltaX = gamma(5) * (maxXt + maxZt);
        float deltaY = gamma(5) * (maxYt + maxZt);

        // Compute $\delta_e$ term for triangle $t$ error bounds
        float deltaE = 2 * (gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);

        // Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
        float maxE = maxComponent(glm::abs(glm::vec3(e0, e1, e2)));
        float deltaT = 3 * (gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) * glm::abs(invDet);
        if (t <= deltaT)
            return false;

        return true;

    }
    bool Triangle::Hit(const Ray& ray, float& tHit, SurfaceInteraction& inter) const {
        const auto& p0 = _mesh->GetPositionAt(_indices[0]);
        const auto& p1 = _mesh->GetPositionAt(_indices[1]);
        const auto& p2 = _mesh->GetPositionAt(_indices[2]);


        glm::vec3 p0t = p0 - glm::vec3(ray._origin);
        glm::vec3 p1t = p1 - glm::vec3(ray._origin);
        glm::vec3 p2t = p2 - glm::vec3(ray._origin);
        int kz = maxDimension(glm::abs(ray._direction));
        // int kx = kz + 1;
        // if (kx == 3) kx = 0;
        // int ky = kx + 1;
        // if (ky == 3) ky = 0;
        int kx = (kz + 1) % 3;
        int ky = (kx + 1) % 3;
        glm::vec3 d = permute(ray._direction, kx, ky, kz);
        p0t = permute(p0t, kx, ky, kz);
        p1t = permute(p2t, kx, ky, kz);
        p2t = permute(p1t, kx, ky, kz);

        float Sx = -d.x / d.z;
        float Sy = -d.y / d.z;
        float Sz = 1.f / d.z;
        p0t.x += Sx * p0t.z;
        p0t.y += Sy * p0t.z;
        p1t.x += Sx * p1t.z;
        p1t.y += Sy * p1t.z;
        p2t.x += Sx * p2t.z;
        p2t.y += Sy * p2t.z;

        //原点分别和三个边求叉乘
        float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
        float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
        float e2 = p0t.x * p1t.y - p0t.y * p1t.x;

        // 不同号时，原点不在三角形内
        if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
            return false;
        //和为0时，原点不在三角形内，在三角形边上
        float det = e0 + e1 + e2;
        if (det == 0)
            return false;

        //由于不相交的情况远多于相交的情况，所以先再次排除一些不相交的情况，以避免重复算浮点数除法
        p0t.z *= Sz;
        p1t.z *= Sz;
        p2t.z *= Sz;
        float t_scaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
        if (det < 0 && (t_scaled >= 0 || t_scaled < ray._t_max * det))
            return false;
        else if (det > 0 && (t_scaled <= 0 || t_scaled > ray._t_max * det))
            return false;

        //用小三角形面积算重心坐标
        float invDet = 1 / det;
        float b0 = e0 * invDet;
        float b1 = e1 * invDet;
        float b2 = e2 * invDet;
        float t = t_scaled * invDet;

        // Compute $\delta_z$ term for triangle $t$ error bounds
        float maxZt = maxComponent(glm::abs(glm::vec3(p0t.z, p1t.z, p2t.z)));
        float deltaZ = gamma(3) * maxZt;

        // Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
        float maxXt = maxComponent(glm::abs(glm::vec3(p0t.x, p1t.x, p2t.x)));
        float maxYt = maxComponent(glm::abs(glm::vec3(p0t.y, p1t.y, p2t.y)));
        float deltaX = gamma(5) * (maxXt + maxZt);
        float deltaY = gamma(5) * (maxYt + maxZt);

        // Compute $\delta_e$ term for triangle $t$ error bounds
        float deltaE = 2 * (gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);

        // Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
        float maxE = maxComponent(glm::abs(glm::vec3(e0, e1, e2)));
        float deltaT = 3 * (gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) * glm::abs(invDet);
        if (t <= deltaT)
            return false;

        // Compute triangle partial derivatives
        glm::vec3 dpdu, dpdv;
        glm::vec2 uv[3];
        if (_mesh->HasUV())
        {
            uv[0] = _mesh->GetUVAt(_indices[0]);
            uv[1] = _mesh->GetUVAt(_indices[1]);
            uv[2] = _mesh->GetUVAt(_indices[2]);
        }
        else
        {
            uv[0] = glm::vec2(0, 0);
            uv[1] = glm::vec2(1, 0);
            uv[2] = glm::vec2(1, 1);
        }

        // Compute deltas for triangle partial derivatives
        glm::vec2 duv02 = uv[0] - uv[2], duv12 = uv[1] - uv[2];
        glm::vec3 dp02 = p0 - p2, dp12 = p1 - p2;
        float determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
        bool degenerateUV = glm::abs(determinant) < 1e-8;
        if (!degenerateUV)
        {
            float invdet = 1 / determinant;
            dpdu = (duv12[1] * dp02 - duv02[1] * dp12) * invdet;
            dpdv = (-duv12[0] * dp02 + duv02[0] * dp12) * invdet;
        }
        if (degenerateUV || glm::length2(glm::cross(dpdu, dpdv)) == 0)
        {
            // Handle zero determinant for triangle partial derivative matrix
            glm::vec3 ng = glm::cross(p2 - p0, p1 - p0);
            // The triangle is actually degenerate; the intersection is bogus.
            if (glm::length2(ng) == 0)
                return false;

            coordinateSystem(glm::normalize(ng), dpdu, dpdv);
        }

        // Interpolate $(u,v)$ parametric coordinates and hit point
        glm::vec3 p_hit = b0 * p0 + b1 * p1 + b2 * p2;
        glm::vec2 uv_hit = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];

        // Fill in _SurfaceInteraction_ from triangle hit
        inter = SurfaceInteraction(p_hit, uv_hit, -ray._direction, dpdu, dpdv, this);

        // Override surface normal in _isect_ for triangle
        inter.n = glm::vec3(glm::normalize(glm::cross(dp02, dp12)));
        tHit = t;

        if (_mesh->HasNormal())
        {
            glm::vec3 ns;
            ns = b0 * _mesh->GetNormalAt(_indices[0]) + b1 * _mesh->GetNormalAt(_indices[1])
                + b2 * _mesh->GetNormalAt(_indices[2]);
            if (glm::length2(ns) > 0)
            {
                ns = glm::normalize(ns);
            }
            else
            {
                ns = inter.n;
            }
            inter.n = ns;
        }

        return true;
    }
}
