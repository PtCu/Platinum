

#ifndef MATH_TRANSFORM_CPP_
#define MATH_TRANSFORM_CPP_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <core/utilities.h>
#include <math/bounds.h>
namespace platinum
{
    class Transform
    {
    public:
        Transform()
            : _trans(glm::mat4(1.f)), _trans_inv(glm::mat4(1.f)) {}

        explicit Transform(const glm::mat4 &m) : _trans(m), _trans_inv(glm::inverse(m)) {}

        Transform(const glm::mat4 &m, const glm::mat4 &m_inv)
            : _trans(m), _trans_inv(m_inv) {}

        Transform(float mat[16])
        {
            _trans = glm::make_mat4(mat);
            _trans_inv = glm::inverse(_trans);
        }

        friend Transform Inverse(const Transform &t) { return Transform(t._trans_inv, t._trans); }

        friend Transform Transpose(const Transform &t) { return Transform(glm::transpose(t._trans), glm::transpose(t._trans)); }

        bool operator==(const Transform &t) const
        {
            return t._trans == _trans && t._trans_inv == _trans_inv;
        }
        bool operator!=(const Transform &t) const
        {
            return t._trans != _trans || t._trans_inv != _trans_inv;
        }
        bool IsIdentity() const
        {
            return _trans == glm::mat4(1.f);
        }
        Transform operator*(const Transform &t2) const
        {
            return Transform(_trans * t2._trans, t2._trans_inv * _trans_inv);
        }

        const glm::mat4 &GetMatrix() const { return _trans; }

        const glm::mat4 &GetInverseMatrix() const { return _trans_inv; }

        /**
         * @brief 作用于向量或点，返回转变后的新向量或点
         * @param  p                待转变的包围盒
         * @param  w                齐次坐标的第四维，最后要除以w
         * @return Vector3f        转变后的向量或点
         */
        Vector3f ExecOn(const Vector3f &p, float w) const;
        /**
         * @brief  作用于包围盒，返回转变后的新包围盒
         * @param  b        待转变的包围盒
         * @return AABB
         */
        Bounds3f ExecOn(const Bounds3f &b) const;

        /**
         * @brief 作用于光线，返回转变后的新光线
         *
         * @param r
         * @return Ray
         */
        Ray ExecOn(const Ray &r) const;

        /**
         * @brief 作用于相交部分，返回转变后的相交部分
         *
         * @param si
         * @return SurfaceInteraction
         */
        SurfaceInteraction ExecOn(const SurfaceInteraction &si) const;

    private:
        glm::mat4 _trans, _trans_inv;
    };

    Transform Translate(const Vector3f &delta);
    Transform Scale(float x, float y, float z);
    Transform Scale(const Vector3f &wrapped);
    Transform RotateX(float theta);
    Transform RotateY(float theta);
    Transform RotateZ(float theta);
    Transform Rotate(float theta, const Vector3f &axis);
    Transform LookAt(const Vector3f &pos, const Vector3f &look, const Vector3f &up);
    Transform Orthographic(float znear, float zfar);
    Transform Perspective(float fov, float znear, float zfar);

}

#endif