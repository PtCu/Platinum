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

#ifndef MATH_TRANSFORM_CPP_
#define MATH_TRANSFORM_CPP_


#include <core/defines.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math/bounds.h>
namespace platinum {
    class Transform {
    public:
        Transform()
            :_trans(glm::mat4(1.f)), _trans_inv(glm::mat4(1.f)) {}
        Transform(const glm::mat4& m) :
            _trans(m), _trans_inv(glm::inverse(m)) {}
        Transform(const glm::mat4& m, const glm::mat4& m_inv)
            :_trans(m), _trans_inv(m_inv) {}
        Transform(float mat[16]) {
            _trans = glm::make_mat4(mat);
            _trans_inv = glm::inverse(_trans);
        }
        friend Transform Inverse(const Transform &t) { return Transform(t._trans_inv, t._trans); }
		friend Transform Transpose(const Transform &t) { return Transform(glm::transpose(t._trans),glm::transpose(t._trans)); }

        bool operator==(const Transform& t)const
        {
            return t._trans == _trans && t._trans_inv == _trans_inv;
        }
        bool operator!=(const Transform& t)const {
            return t._trans != _trans || t._trans_inv != _trans_inv;
        }
        bool IsIdentity()const {
            return _trans == glm::mat4(1.f);
        }
        Transform operator*(const Transform& t2)const {
            return Transform(_trans * t2._trans, _trans_inv * t2._trans_inv);
        }
        const glm::mat4& GetMatrix()const { return _trans; }
        const glm::mat4& GetInverseMatrix()const { return _trans_inv; }

        /**
         * @brief 作用于向量，转变其坐标
         * @param  p                待转变的包围盒
         * @param  w                齐次坐标的第四维，最后要除以w
         * @return glm::vec3 
         */
        glm::vec3 ExecOn(const glm::vec3& p, float w)const;
        /**
         * @brief  作用于包围盒，转变其坐标
         * @param  b        待转变的包围盒
         * @return AABB 
         */
        AABB ExecOn(const AABB& b)const;
    private:
        glm::mat4 _trans, _trans_inv;

    };


    Transform Translate(const glm::vec3& delta);
    Transform Scale(float x, float y, float z);
    Transform RotateX(float theta);
    Transform RotateY(float theta);
    Transform RotateZ(float theta);
    Transform Rotate(float theta, const glm::vec3& axis);
    Transform LookAt(const glm::vec3& pos, const glm::vec3& look, const glm::vec3& up);
    Transform Orthographic(float znear, float zfar);
    Transform Perspective(float fov, float znear, float zfar);




}

#endif