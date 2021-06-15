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
#include "glog/logging.h"
#include "nlohmann/json.hpp"

#if defined(_MSC_VER)
#define NOMINMAX
#endif

#define ALLOCA(TYPE, COUNT) (TYPE *)alloca((COUNT) * sizeof(TYPE))
// Platform-specific definitions
#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS_OS
#endif
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
    static constexpr Float Pi = 3.14159265358979323846;
    static constexpr Float _2Pi = 3.14159265358979323846 * 2;
    static constexpr Float InvPi = 0.31830988618379067154;
    static constexpr Float Inv2Pi = 0.15915494309189533577;
    static constexpr Float Inv4Pi = 0.07957747154594766788;
    static constexpr Float PiOver2 = 1.57079632679489661923;
    static constexpr Float PiOver4 = 0.78539816339744830961;
    static constexpr Float Sqrt2 = 1.41421356237309504880;

    class Ray;
    class Film;
    class BSDF;
    class BxDF;
    class Light;
    class Shape;
    class Scene;
    class Camera;
    class Hitable;
    class HitableObject;
    class FilmTile;
    class Sampler;
    class Material;
    class AreaLight;
    class Transform;
    class Integrator;
    class CameraSample;
    class RGBSpectrum;
    class Interaction;
    class Distribution1D;
    class VisibilityTester;
    class SurfaceInteraction;

    class MemoryArena;

    using Spectrum = RGBSpectrum;
    // TransportMode Declarations
    enum class TransportMode
    {
        Radiance,
        Importance
    };
    typedef nlohmann::json nloJson;

}

#endif