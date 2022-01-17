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

//core/
// Global Include Files

#ifndef CORE_DEFINES_H_
#define CORE_DEFINES_H_

#include <type_traits>
#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <stack>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <float.h>
#include <typeinfo>
#include <thread>
#include <mutex>
#include <glm/glm.hpp>
#include <glog/logging.h>
namespace platinum
{
    // Platform-specific definitions
#if defined(_WIN32) || defined(_WIN64)
#define PLT_IS_WINDOWS
#endif

    static constexpr float ShadowEpsilon = 0.0001f;
    static constexpr float Pi = 3.14159265358979323846f;
    static constexpr float InvPi = 0.31830988618379067154f;
    static constexpr float Inv2Pi = 0.15915494309189533577f;
    static constexpr float Inv4Pi = 0.07957747154594766788f;
    static constexpr float PiOver2 = 1.57079632679489661923f;
    static constexpr float PiOver4 = 0.78539816339744830961f;
    static constexpr float Sqrt2 = 1.41421356237309504880f;
    static constexpr float EPSILON = 0.0005f;
    static const float OneMinusEpsilon = 0.99999994;
    constexpr static float MaxFloat = std::numeric_limits<float>::max();
    constexpr static float Infinity = std::numeric_limits<float>::infinity();
    constexpr static float MachineEpsilon = std::numeric_limits<float>::epsilon() * 0.5f;

    inline float lerp(float t, float v1, float v2) { return (1 - t) * v1 + t * v2; }
    inline float gamma(int n) { return (n * MachineEpsilon) / (1 - n * MachineEpsilon); }

    inline float minComponent(const glm::vec3& v) { return glm::min(v.x, glm::min(v.y, v.z)); }


    inline float maxComponent(const glm::vec3& v) { return glm::max(v.x, glm::max(v.y, v.z)); }


    inline int maxDimension(const glm::vec3& v) { return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2); }


    inline glm::vec3 permute(const glm::vec3& v, int x, int y, int z) { return glm::vec3(v[x], v[y], v[z]); }

    inline glm::vec3 faceforward(const glm::vec3& n, const glm::vec3& v) { return (glm::dot(n, v) < 0.f) ? -n : n; }

    //正交化，用于局部坐标
    //v1必须已经被归一化过
    inline void coordinateSystem(const glm::vec3& v1, glm::vec3& v2, glm::vec3& v3)
    {
        if (glm::abs(v1.x) > glm::abs(v1.y))
            v2 = glm::vec3(-v1.z, 0, v1.x) / glm::sqrt(v1.x * v1.x + v1.z * v1.z);
        else
            v2 = glm::vec3(0, v1.z, -v1.y) / glm::sqrt(v1.y * v1.y + v1.z * v1.z);
        v3 = cross(v1, v2);
    }
    
    class Camera;
    class Material;
    class Scene;
    class Ray;
    class AABB;
    class Texture;
    class Integrator;
    class TiledIntegrator;
    class Film;
    class BSDF;
    class BxDF;
    class VisibilityTester;
    class Light;
    class Sampler;
    class Fresnel;
    class Interaction;
    class SurfaceInteraction;
    struct CameraSample;
    class Filter;
    class Transform;
    class AreaLight;
    class Primitive;
    class GeometricPrimitive;
    class Aggregate;
    class Shape;
    class AABB;

} // namespace platinum
#endif