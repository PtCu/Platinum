#ifndef CORE_PLATINUM_H_
#define CORE_PLATINUM_H_

#include <iostream>
#include <string>
#include <assert.h>
#include <cmath>
#include <stdlib.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <map>
#include "glm/glm.hpp"
#include <glog/logging.h>
#include "nlohmann/json.hpp"

#if defined(_MSC_VER)
#define NOMINMAX
#endif

#define PLATINUM_BEGIN \
    namespace platinum \
    {
#define PLATINUM_END }

typedef nlohmann::json nloJson;

namespace platinum
{
#ifdef FLOAT_AS_DOUBLE
    typedef double Float;
#else
    typedef float Float;
#endif

    static constexpr Float MaxFloat = std::numeric_limits<Float>::max();
    static constexpr Float Infinity = std::numeric_limits<Float>::infinity();
    static constexpr Float MachineEpsilon = std::numeric_limits<Float>::epsilon() * 0.5;
    static constexpr Float RayEpsilon = MachineEpsilon * 1500;
    static constexpr Float ShadowEpsilon = RayEpsilon * 10;
    static constexpr Float Epsilon = std::numeric_limits<Float>::epsilon();
    //static constexpr Float ShadowEpsilon = 0.0001f;
    static constexpr Float Pi = 3.14159265358979323846;
    static constexpr Float _2Pi = 3.14159265358979323846 * 2;
    static constexpr Float InvPi = 0.31830988618379067154;
    static constexpr Float Inv2Pi = 0.15915494309189533577;
    static constexpr Float Inv4Pi = 0.07957747154594766788;
    static constexpr Float PiOver2 = 1.57079632679489661923;
    static constexpr Float PiOver4 = 0.78539816339744830961;
    static constexpr Float Sqrt2 = 1.41421356237309504880;

    template <typename T>
    using Vector2 = glm::vec<2, T>;

    template <typename T>
    using Vector3 = glm::vec<3, T>;

    typedef Vector2<Float> Vector2f;
    typedef Vector2<int> Vector2i;
    typedef Vector3<Float> Vector3f;
    typedef Vector3<int> Vector3i;

    template <typename T>
    class Point2;

    template <typename T>
    class Point3;

    class Primitive;
    class Transform;
    class Ray;
    struct Interaction;
    class Shape;
    class Aggregate;
    class Bounds3f;
}

#endif