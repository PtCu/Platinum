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

#include <core/camera.h>

namespace platinum
{
   
    Camera::Camera(const glm::vec3& lookfrom, const glm::vec3& lookat, const glm::vec3& vup, float vfov, float aspect, float aperture, float focus_dist,float t0,float t1) : origin_(lookfrom), lens_radius(aperture/2),_t0(t0),_t1(t1)
    {
        if (focus_dist == -1.0f)
            focus_dist = glm::distance(lookfrom, lookat);

        float theta = vfov / 180.0f * PI;
        float height = 2 * focus_dist * tan(theta / 2);
        float width = aspect * height;
        origin_ = lookfrom;
        front = glm::normalize(lookat - lookfrom);
        right = glm::normalize(glm::cross(vup, -front));
        up = glm::cross(-front, right);
        lower_left_corner = origin_ + focus_dist * front - width / 2 * right - height / 2 * up;
        horizontal = width * right;
        vertical = height * up;
    }

    Ray Camera::GetRay(float s, float t) const
    {
        Ray ray(origin_, lower_left_corner + s * horizontal + t * vertical - origin_);
        float time = _t0 + Random::UniformFloat() * (_t1 - _t0);
        ray.SetTime(time);
        return ray;
    }

    void ProjectiveCamera::Initialize() {
        int width = _film->GetWidth();
        int height = _film->GetHeight();
        glm::vec2 p_min, p_max;
        float ratio = static_cast<float>(width) / height;
        if (ratio > 1.f) {
            p_min.x = -ratio;
            p_min.y = -1.f;
            p_max.x = ratio;
            p_max.y = 1.f;
        }
        else {
            p_min.x = -1.f;
            p_min.y = -1.f / ratio;
            p_max.x = 1.f;
            p_max.y = 1.f / ratio;
        }
        // 屏幕空间(0,0)为胶片平面矩形的中点，范围由p_min和p_max两个点决定
        // 自左向右乘：先将左下角平移至原点，再缩放到[0,1]x[0,1]，再缩放到[0,width]x[0,height]
        _screen2raster = Scale(width, height, 1) *
            Scale(1 / (p_max.x - p_min.x),
                1 / (p_min.y - p_max.y), 1) *
            Translate(glm::vec3(-p_min.x, -p_max.y, 0));
        _raster2screen = Inverse(_screen2raster);
        _raster2camera = Inverse(_camera2sreen) * _raster2screen;
    }
}
