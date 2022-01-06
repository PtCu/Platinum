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

#ifndef LIGHT_AREA_LIGHT_H_
#define LIGHT_AREA_LIGHT_H_

#include <core/light.h>
#include <math/transform.h>
#include <core/interaction.h>

namespace platinum {
    class DiffuseAreaLight :public AreaLight {
    public:
        DiffuseAreaLight(const Transform* light2world, const glm::vec3& Le, int n_samples);
        virtual glm::vec3 L(const Interaction& inter, const glm::vec3& w)const override {
            return glm::vec3(0);
        }
    private:
        glm::vec3 _Lemit;
        float _area;
    };
}


#endif