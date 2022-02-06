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

#ifndef CORE_FILTER_H_
#define CORE_FILTER_H_

#include <core/utilities.h>

namespace platinum
{
    class Filter
    {
    public:
        virtual ~Filter() = default;

        Filter(const Vector2f &radius)
            : m_radius(radius), m_invRadius(Vector2f(1 / radius.x, 1 / radius.y)) {}

        virtual float Evaluate(const Vector2f &p) const = 0;

        const Vector2f m_radius, m_invRadius;
    };
}

#endif