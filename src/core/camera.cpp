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

    void ProjectiveCamera::Initialize()
    {
        Bounds2f screen;
        auto res = _film->getResolution();
        float frame = (float)(res.x) / res.y;
        if (frame > 1.f)
        {
            screen._p_min.x = -frame;
            screen._p_max.x = frame;
            screen._p_min.y = -1.f;
            screen._p_max.y = 1.f;
        }
        else
        {
            screen._p_min.x = -1.f;
            screen._p_max.x = 1.f;
            screen._p_min.y = -1.f / frame;
            screen._p_max.y = 1.f / frame;
        }

        _screen2raster = Scale(res.x, res.y, 1) *
                         Scale(1 / (screen._p_max.x - screen._p_min.x),
                               1 / (screen._p_min.y - screen._p_max.y), 1) *
                         Translate(Vector3f(-screen._p_min.x, -screen._p_max.y, 0));
        _raster2screen = Inverse(_screen2raster);
        _raster2camera = Inverse(_camera2screen) * _raster2screen;
    }
}
