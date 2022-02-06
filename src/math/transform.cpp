// Copyright 2022 ptcup
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <math/transform.h>
#include <math/bounds.h>
#include <core/interaction.h>

namespace platinum
{

    Ray Transform::ExecOn(const Ray &r) const
    {
        Vector3f o = this->ExecOn(r._origin, 1.f);
        Vector3f d = this->ExecOn(r._direction, 0.f);
        return Ray(o, d, r._t_max);
    }
    Vector3f Transform::ExecOn(const Vector3f &p, float w) const
    {
        glm::vec4 ret = _trans * glm::vec4(p.x, p.y, p.z, w);

        //w==0 --> vector
        //w==1 --> point
        if (w == 0.f || w == 1.f)
            return Vector3f(ret.x, ret.y, ret.z);
        else
            return Vector3f(ret.x, ret.y, ret.z) / ret.w;
    }
    Bounds3f Transform::ExecOn(const Bounds3f &b) const
    {
        //每个顶点都进行转化，最后合并
        const auto &mat = *this;
        Bounds3f ret(mat.ExecOn(b._p_min, 1.f));
        ret = UnionBounds(ret, mat.ExecOn(Vector3f(b._p_max.x, b._p_min.y, b._p_min.z), 1.0f));
        ret = UnionBounds(ret, mat.ExecOn(Vector3f(b._p_min.x, b._p_max.y, b._p_min.z), 1.0f));
        ret = UnionBounds(ret, mat.ExecOn(Vector3f(b._p_min.x, b._p_min.y, b._p_max.z), 1.0f));
        ret = UnionBounds(ret, mat.ExecOn(Vector3f(b._p_min.x, b._p_max.y, b._p_max.z), 1.0f));
        ret = UnionBounds(ret, mat.ExecOn(Vector3f(b._p_max.x, b._p_max.y, b._p_min.z), 1.0f));
        ret = UnionBounds(ret, mat.ExecOn(Vector3f(b._p_max.x, b._p_min.y, b._p_max.z), 1.0f));
        ret = UnionBounds(ret, mat.ExecOn(Vector3f(b._p_max.x, b._p_max.y, b._p_max.z), 1.0f));
        return ret;
    }

    SurfaceInteraction Transform::ExecOn(const SurfaceInteraction &si) const
    {
        SurfaceInteraction inter;
        //变换相交点
        inter.p = this->ExecOn(si.p, 1.f);

        inter.n = glm::normalize(this->ExecOn(si.n, 0.f));
        inter.wo = glm::normalize(this->ExecOn(si.wo, 0.f));
        inter._uv = si._uv;
        inter._shape = si._shape;
        inter._dpdu = this->ExecOn(si._dpdu, 0.f);
        inter._dpdv = this->ExecOn(si._dpdv, 0.f);
        inter._hitable = si._hitable;
        return inter;
    }
    Transform Translate(const Vector3f &delta)
    {
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), delta);
        glm::mat4 transInv = glm::translate(glm::mat4(1.0f), -delta);
        return Transform(trans, transInv);
    }

    Transform Scale(float x, float y, float z)
    {
        glm::mat4 trans = glm::scale(glm::mat4(1.0f), Vector3f(x, y, z));
        glm::mat4 transInv = glm::scale(glm::mat4(1.0f), Vector3f(1 / x, 1 / y, 1 / z));
        return Transform(trans, transInv);
    }

    Transform RotateX(float theta)
    {
        glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians(theta), Vector3f(1, 0, 0));
        glm::mat4 transInv = inverse(trans);
        return Transform(trans, transInv);
    }

    Transform RotateY(float theta)
    {
        glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians(theta), Vector3f(0, 1, 0));
        glm::mat4 transInv = inverse(trans);
        return Transform(trans, transInv);
    }

    Transform RotateZ(float theta)
    {
        glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians(theta), Vector3f(0, 0, 1));
        glm::mat4 transInv = inverse(trans);
        return Transform(trans, transInv);
    }

    Transform Rotate(float theta, const Vector3f &axis)
    {
        glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians(theta), axis);
        glm::mat4 transInv = inverse(trans);
        return Transform(trans, transInv);
    }

    Transform LookAt(const Vector3f &pos, const Vector3f &look, const Vector3f &up)
    {
        glm::mat4 worldToCamera = glm::lookAt(pos, look, up);
        return Transform(worldToCamera, inverse(worldToCamera));
    }

    Transform Orthographic(float znear, float zfar)
    {
        return Scale(1, 1, 1 / (zfar - znear)) * Translate(Vector3f(0, 0, -znear));
    }

    Transform Perspective(float fov, float n, float f)
    {
        // Perform projective divide for perspective projection
        glm::mat4 persp(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, f / (f - n), 1,
            0, 0, -f * n / (f - n), 0);
        // Scale canonical perspective view to specified field of view
        float invTanAng = 1 / glm::tan(glm::radians(fov) / 2);
        return Scale(invTanAng, invTanAng, 1) * Transform(persp);
    }

}