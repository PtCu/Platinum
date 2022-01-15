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

#include <core/sampler.h>

namespace platinum {
    glm::vec3 UniformSampleHemisphere(const glm::vec2& u)
    {
        float z = u[0];
        float r = glm::sqrt(glm::max((float)0, (float)1. - z * z));
        float phi = 2 * Pi * u[1];
        return glm::vec3(r * glm::cos(phi), r * glm::sin(phi), z);
    }

    float UniformHemispherePdf() { return Inv2Pi; }

    glm::vec3 UniformSampleSphere(const glm::vec2& u)
    {
        float z = 1 - 2 * u[0];
        float r = glm::sqrt(glm::max((float)0, (float)1 - z * z));
        float phi = 2 * Pi * u[1];
        return glm::vec3(r * glm::cos(phi), r * glm::sin(phi), z);
    }

    float UniformSpherePdf() { return Inv4Pi; }

    float UniformConePdf(float cosThetaMax) { return 1 / (2 * Pi * (1 - cosThetaMax)); }

    glm::vec2 ConcentricSampleDisk(const glm::vec2& u)
    {
        // Map uniform random numbers to $[-1,1]^2$
        glm::vec2 uOffset = 2.f * u - glm::vec2(1, 1);

        // Handle degeneracy at the origin
        if (uOffset.x == 0 && uOffset.y == 0)
            return glm::vec2(0, 0);

        // Apply concentric mapping to point
        float theta, r;
        if (glm::abs(uOffset.x) > glm::abs(uOffset.y))
        {
            r = uOffset.x;
            theta = PiOver4 * (uOffset.y / uOffset.x);
        }
        else
        {
            r = uOffset.y;
            theta = PiOver2 - PiOver4 * (uOffset.x / uOffset.y);
        }
        return r * glm::vec2(glm::cos(theta), glm::sin(theta));
    }

    glm::vec2 UniformSampleTriangle(const glm::vec2& u)
    {
        float su0 = glm::sqrt(u[0]);
        return glm::vec2(1 - su0, u[1] * su0);
    }

}