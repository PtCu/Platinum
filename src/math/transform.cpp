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

namespace platinum {
    class Transform {
    public:
        Transform()
        :_trans(glm::mat4(1.f)),_trans_inv(glm::mat4(1.f)){}
    private:
        glm::mat4 _trans, _trans_inv;

    };
}

#endif