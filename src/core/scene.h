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

#ifndef CORE_WORLD_H_
#define CORE_WORLD_H_

#include <core/utilities.h>
#include <core/primitive.h>
#include <core/light.h>
#include <functional>


namespace platinum
{
    class Scene
    {
    public:
        Scene(const Ptr<Aggregate> &aggre, const std::vector<Ptr<Light>> &lights)
            : _aggres(aggre), _lights(lights)
        {
            _worldbound = _aggres->WorldBound();
            for (const auto &light : _lights)
            {
                light->Preprocess(*this);
                if (light->_flags & (int)LightFlags::LightInfinite)
                    _infinite_lights.push_back(light);
            }
        }

        bool Hit(const Ray &ray, SurfaceInteraction &inter) const;

        bool Hit(const Ray &ray) const;

        const Bounds3f &WorldBound() const { return _worldbound; }

        std::vector<Ptr<Light>> _lights;
        std::vector<Ptr<Light>> _infinite_lights;

    private:
        Ptr<Aggregate> _aggres;
        Bounds3f _worldbound;
    };

} // namespace platinum

#endif
