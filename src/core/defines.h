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

    class Camera;
    class Material;
    class Scene;
    class Ray;
    class Object;
    struct HitRecord;
    struct HitRst;
    class AABB;
    class Texture;
    class ConstTexture;
    class CheckerTexture;
    class BVHAccel;
    struct BVH_Node;
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
    class Hitable;
    class HitableObject;
    class HitableAggregate;
    class Shape;
} // namespace platinum
#endif