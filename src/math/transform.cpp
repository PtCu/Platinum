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

namespace platinum {


    Ray Transform::ExecOn(const Ray& r)const {
        glm::vec3 o = this->ExecOn(r._origin, 1.f);
        glm::vec3 d = this->ExecOn(r._direction, 0.f);
        return Ray(o, d, r._t_max);
    }
    glm::vec3 Transform::ExecOn(const glm::vec3& p, float w)const {
        glm::vec4 ret = _trans * glm::vec4(p.x, p.y, p.z, w);
        //w==0 --> vector
        if (w == 0.f)
            return glm::vec3(ret.x, ret.y, ret.z);
        //w==1 --> point
        if (ret.w == 1)
            return glm::vec3(ret.x, ret.y, ret.z);
        else
            return glm::vec3(ret.x, ret.y, ret.z) / ret.w;

    }
    AABB Transform::ExecOn(const AABB& b)const {
        //每个顶点都进行转化，最后合并
        const auto& mat = *this;
        AABB ret(mat.ExecOn(b._p_min, 1.f));
        ret.UnionWith(mat.ExecOn(glm::vec3(b._p_max.x, b._p_min.y, b._p_min.z), 1.0f));
        ret.UnionWith(mat.ExecOn(glm::vec3(b._p_min.x, b._p_max.y, b._p_min.z), 1.0f));
        ret.UnionWith(mat.ExecOn(glm::vec3(b._p_min.x, b._p_min.y, b._p_max.z), 1.0f));
        ret.UnionWith(mat.ExecOn(glm::vec3(b._p_min.x, b._p_max.y, b._p_max.z), 1.0f));
        ret.UnionWith(mat.ExecOn(glm::vec3(b._p_max.x, b._p_max.y, b._p_min.z), 1.0f));
        ret.UnionWith(mat.ExecOn(glm::vec3(b._p_max.x, b._p_min.y, b._p_max.z), 1.0f));
        ret.UnionWith(mat.ExecOn(glm::vec3(b._p_max.x, b._p_max.y, b._p_max.z), 1.0f));
        return ret;
    }
    Transform Translate(const glm::vec3& delta)
    {
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), delta);
        glm::mat4 transInv = glm::translate(glm::mat4(1.0f), -delta);
        return Transform(trans, transInv);
    }

    Transform Scale(float x, float y, float z)
    {
        glm::mat4 trans = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
        glm::mat4 transInv = glm::scale(glm::mat4(1.0f), glm::vec3(1 / x, 1 / y, 1 / z));
        return Transform(trans, transInv);
    }

    Transform RotateX(float theta)
    {
        glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians(theta), glm::vec3(1, 0, 0));
        glm::mat4 transInv = inverse(trans);
        return Transform(trans, transInv);
    }

    Transform RotateY(float theta)
    {
        glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians(theta), glm::vec3(0, 1, 0));
        glm::mat4 transInv = inverse(trans);
        return Transform(trans, transInv);
    }

    Transform RotateZ(float theta)
    {
        glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians(theta), glm::vec3(0, 0, 1));
        glm::mat4 transInv = inverse(trans);
        return Transform(trans, transInv);
    }

    Transform Rotate(float theta, const glm::vec3& axis)
    {
        glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians(theta), axis);
        glm::mat4 transInv = inverse(trans);
        return Transform(trans, transInv);
    }

    Transform LookAt(const glm::vec3& pos, const glm::vec3& look, const glm::vec3& up)
    {
        glm::mat4 worldToCamera = glm::lookAt(pos, look, up);
        return Transform(worldToCamera, inverse(worldToCamera));
    }

    Transform Orthographic(float znear, float zfar)
    {
        return Scale(1, 1, 1 / (zfar - znear)) * Translate(glm::vec3(0, 0, -znear));
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