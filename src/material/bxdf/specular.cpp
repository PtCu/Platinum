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

#include <material/bxdf/specular.h>

namespace platinum {
    glm::vec3 SpecularReflection::SampleF(const glm::vec3& wo, glm::vec3& wi, const glm::vec2& sample, float& pdf, BxDFType& sampleType)const {
        wi = glm::vec3(-wo.x, -wo.y, wo.z);
        pdf = 1.f;
        return _fresnel->Evaluate(wi.z) * _R / glm::abs(wi.z);
    }
}