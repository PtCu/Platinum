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
#ifndef ACCELERATOR_LINEAR_AGGREGATE_CPP_
#define ACCELERATOR_LINEAR_AGGREGATE_CPP_

#include <core/primitive.h>

namespace platinum
{
    class LinearAggregate final : public Aggregate
    {
    public:
        LinearAggregate()=default;

        LinearAggregate(const std::vector<Ptr<Primitive>> &);

        virtual bool Hit(const Ray &ray) const override;

        virtual bool Hit(const Ray &ray, SurfaceInteraction &inter) const;

        virtual Bounds3f WorldBound() const override { return _world_bounds; }

        std::string ToString() const { return "LinearAggregate"; }
    };
}

#endif
