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

    void ProjectiveCamera::Initialize() {
        auto res = _film->getResolution();
        glm::vec2 p_min, p_max;
        float ratio = static_cast<float>(res.x) / res.y;
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
        // 自左向右乘：先将左下角平移至原点，再缩放到[0,1]x[0,1]，再缩放到[0,res.x]x[0,res.y]
        _screen2raster = Scale(res.x, res.y, 1) *
            Scale(1 / (p_max.x - p_min.x),
                1 / (p_min.y - p_max.y), 1) *
            Translate(glm::vec3(-p_min.x, -p_max.y, 0));
        _raster2screen = Inverse(_screen2raster);
        _raster2camera = Inverse(_camera2sreen) * _raster2screen;
    }
}
