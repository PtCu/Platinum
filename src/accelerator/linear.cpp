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

#include <accelerator/linear.h>

namespace platinum {
    LinearAggregate::LinearAggregate(const std::vector < std::shared_ptr<Primitive>>& hitables)
        :Aggregate(hitables)
    {
        for (const auto& hitable : _hitables) {
            _world_bounds = UnionBounds(_world_bounds, hitable->WorldBound());
        }
    }

    bool LinearAggregate::Hit(const Ray& ray)const {
        for (const auto& hitble : _hitables) {
            if (hitble->Hit(ray)) {
                return true;
            }
        }
        return false;
    }

    bool LinearAggregate::Hit(const Ray& ray, SurfaceInteraction& inter)const {
        SurfaceInteraction tmp_inter;
        bool is_hit = false;
        for (const auto& hitble : _hitables) {
            if (hitble->Hit(ray, tmp_inter)) {
                
                is_hit = true;
                inter = tmp_inter;
            }
        }
        return is_hit;
    }
}