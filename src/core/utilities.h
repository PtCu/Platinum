

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
#include <string>
#include <typeinfo>
#include <thread>
#include <mutex>
#include <initializer_list>

// #define GLM_FORCE_AVX
// #define GLM_FORCE_ALIGNED
#define GLM_FORCE_INTRINSICS
#include <glm/glm.hpp>
#include <glog/logging.h>
    namespace platinum
{
    // Platform-specific definitions
#if defined(_WIN32) || defined(_WIN64)
#define PLT_IS_WINDOWS
#endif

#define ALLOCA(TYPE, COUNT) (TYPE *)alloca((COUNT) * sizeof(TYPE))

    static constexpr float ShadowEpsilon = 0.0001f;
    static constexpr float Pi = 3.14159265358979323846f;
    static constexpr float InvPi = 0.31830988618379067154f;
    static constexpr float Inv2Pi = 0.15915494309189533577f;
    static constexpr float Inv4Pi = 0.07957747154594766788f;
    static constexpr float PiOver2 = 1.57079632679489661923f;
    static constexpr float PiOver4 = 0.78539816339744830961f;
    static constexpr float Sqrt2 = 1.41421356237309504880f;
    static constexpr float EPSILON = 0.0005f;
    static constexpr float OneMinusEpsilon = 0.99999994f;
    static constexpr float MaxFloat = std::numeric_limits<float>::max();
    static constexpr float Infinity = std::numeric_limits<float>::infinity();
    static constexpr float MachineEpsilon = std::numeric_limits<float>::epsilon() * 0.5f;

    class Camera;
    class Material;
    class Scene;
    class Ray;
    template <typename T>
    class Texture;
    class Integrator;
    class SamplerIntegrator;
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
    struct FilmTilePixel;
    class FilmTile;
    class RGBSpectrum;
    class Object;
    class MemoryArena;

    using Spectrum = RGBSpectrum;

    using Byte = unsigned char;

    template <typename T>
    using Vector3 = glm::vec<3, T>;

    template <typename T>
    using Vector2 = glm::vec<2, T>;

    using Vector3f = Vector3<float>;
    using Vector3i = Vector3<int>;
    using Vector2f = Vector2<float>;
    using Vector2i = Vector2<int>;

    template <typename T>
    using Ptr = std::shared_ptr<T>;

    template <typename T>
    using PtrC = std::shared_ptr<const T>;

    template <typename T>
    using UPtr = std::unique_ptr<T>;

    template <typename T>
    using UPtrC = std::unique_ptr<const T>;

    template <typename T>
    using WPtr = std::weak_ptr<T>;

    template <typename T>
    bool operator<(const WPtr<T> &lhs, const WPtr<T> &rhs)
    {
        return lhs.lock() < rhs.lock();
    }

    template <typename T>
    using WPtrC = std::weak_ptr<const T>;

    template <typename T, typename U, typename V>
    inline T clamp(T val, U low, V high)
    {
        if (val < low)
            return low;
        else if (val > high)
            return high;
        else
            return val;
    }

    inline float lerp(float t, float v1, float v2) { return (1 - t) * v1 + t * v2; }

    inline float gamma(int n) { return (n * MachineEpsilon) / (1 - n * MachineEpsilon); }

    inline float gammaCorrect(float value)
    {
        if (value <= 0.0031308f)
            return 12.92f * value;
        return 1.055f * glm::pow(value, (float)(1.f / 2.4f)) - 0.055f;
    }

    inline float inverseGammaCorrect(float value)
    {
        if (value <= 0.04045f)
            return value * 1.f / 12.92f;
        return glm::pow((value + 0.055f) * 1.f / 1.055f, (float)2.4f);
    }
    inline float minComponent(const Vector3f &v) { return glm::min(v.x, glm::min(v.y, v.z)); }

    inline float maxComponent(const Vector3f &v) { return glm::max(v.x, glm::max(v.y, v.z)); }

    inline int maxDimension(const Vector3f &v) { return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2); }

    inline Vector3f permute(const Vector3f &v, int x, int y, int z) { return Vector3f(v[x], v[y], v[z]); }

    template <typename T>
    inline glm::vec<3, T> abs(const glm::vec<3, T> &p)
    {
        return glm::vec<3, T>(glm::abs(p.x), glm::abs(p.y), glm::abs(p.z));
    }
    inline Vector3f faceforward(const Vector3f &n, const Vector3f &v) { return (glm::dot(n, v) < 0.f) ? -n : n; }

    inline bool SameHemisphere(const Vector3f &w, const Vector3f &wp) { return w.z * wp.z > 0; }
    //正交化，用于局部坐标
    //v1必须已经被归一化过
    inline void coordinateSystem(const Vector3f &v1, Vector3f &v2, Vector3f &v3)
    {
        if (glm::abs(v1.x) > glm::abs(v1.y))
            v2 = Vector3f(-v1.z, 0, v1.x) / glm::sqrt(v1.x * v1.x + v1.z * v1.z);
        else
            v2 = Vector3f(0, v1.z, -v1.y) / glm::sqrt(v1.y * v1.y + v1.z * v1.z);
        v3 = glm::cross(v1, v2);
    }
    inline Vector3f sphericalDirection(float sinTheta, float cosTheta, float phi)
    {
        return Vector3f(sinTheta * glm::cos(phi), sinTheta * glm::sin(phi), cosTheta);
    }

    inline Vector3f sphericalDirection(float sinTheta, float cosTheta, float phi,
                                       const Vector3f &x, const Vector3f &y, const Vector3f &z)
    {
        return sinTheta * glm::cos(phi) * x + sinTheta * glm::sin(phi) * y + cosTheta * z;
    }

    template <typename Predicate>
    int findInterval(int size, const Predicate &pred)
    {
        int first = 0, len = size;
        while (len > 0)
        {
            int half = len >> 1, middle = first + half;
            // Bisect range based on value of _pred_ at _middle_
            if (pred(middle))
            {
                first = middle + 1;
                len -= half + 1;
            }
            else
            {
                len = half;
            }
        }
        return clamp(first - 1, 0, size - 2);
    }

    inline uint32_t floatToBits(float f)
    {
        uint32_t ui;
        memcpy(&ui, &f, sizeof(float));
        return ui;
    }

    inline float bitsToFloat(uint32_t ui)
    {
        float f;
        memcpy(&f, &ui, sizeof(uint32_t));
        return f;
    }

    inline uint64_t floatToBits(double f)
    {
        uint64_t ui;
        memcpy(&ui, &f, sizeof(double));
        return ui;
    }

    inline double bitsToFloat(uint64_t ui)
    {
        double f;
        memcpy(&f, &ui, sizeof(uint64_t));
        return f;
    }

    inline void stringPrintfRecursive(std::string *s, const char *fmt)
    {
        const char *c = fmt;
        // No args left; make sure there aren't any extra formatting
        // specifiers.
        while (*c)
        {
            if (*c == '%')
            {
                CHECK_EQ(c[1], '%');
                ++c;
            }
            *s += *c++;
        }
    }

    // 1. Copy from fmt to *s, up to the next formatting directive.
    // 2. Advance fmt past the next formatting directive and return the
    //    formatting directive as a string.
    inline std::string copyToFormatString(const char **fmt_ptr, std::string *s)
    {
        const char *&fmt = *fmt_ptr;
        while (*fmt)
        {
            if (*fmt != '%')
            {
                *s += *fmt;
                ++fmt;
            }
            else if (fmt[1] == '%')
            {
                // "%%"; let it pass through
                *s += '%';
                *s += '%';
                fmt += 2;
            }
            else
                // fmt is at the start of a formatting directive.
                break;
        }

        std::string nextFmt;
        if (*fmt)
        {
            do
            {
                nextFmt += *fmt;
                ++fmt;
                // Incomplete (but good enough?) test for the end of the
                // formatting directive: a new formatting directive starts, we
                // hit whitespace, or we hit a comma.
            } while (*fmt && *fmt != '%' && !isspace(*fmt) && *fmt != ',' &&
                     *fmt != '[' && *fmt != ']' && *fmt != '(' && *fmt != ')');
        }

        return nextFmt;
    }

    template <typename T>
    inline std::string formatOne(const char *fmt, T v)
    {
        // Figure out how much space we need to allocate; add an extra
        // character for the '\0'.
        size_t size = snprintf(nullptr, 0, fmt, v) + 1;
        std::string str;
        str.resize(size);
        snprintf(&str[0], size, fmt, v);
        str.pop_back(); // remove trailing NUL
        return str;
    }

    // General-purpose version of stringPrintfRecursive; add the formatted
    // output for a single StringPrintf() argument to the final result string
    // in *s.
    template <typename T, typename... Args>
    inline void stringPrintfRecursive(std::string *s, const char *fmt, T v, Args... args)
    {
        std::string nextFmt = copyToFormatString(&fmt, s);
        *s += formatOne(nextFmt.c_str(), v);
        stringPrintfRecursive(s, fmt, args...);
    }

    // Special case of StringPrintRecursive for float-valued arguments.
    template <typename... Args>
    inline void stringPrintfRecursive(std::string *s, const char *fmt, float v, Args... args)
    {
        std::string nextFmt = copyToFormatString(&fmt, s);
        if (nextFmt == "%f")
            // Always use enough precision so that the printed value gives
            // the exact floating-point value if it's used to initialize a
            // float.
            // https://randomascii.wordpress.com/2012/03/08/float-precisionfrom-zero-to-100-digits-2/
            *s += formatOne("%.9g", v);
        else
            // If a specific formatting string other than "%f" was specified,
            // just use that.
            *s += formatOne(nextFmt.c_str(), v);

        // Go forth and print the next arg.
        stringPrintfRecursive(s, fmt, args...);
    }

    template <typename... Args>
    inline std::string StringPrintf(const char *fmt, Args... args)
    {
        std::string ret;
        stringPrintfRecursive(&ret, fmt, args...);
        return ret;
    }

    template <typename T>
    std::ostream &operator<<(std::ostream &os, const glm::vec<3, T> &v)
    {
        os << "[" << v.x << "," << v.y << "," << v.z << "]";
        return os;
    }

    template <typename T>
    std::ostream &operator<<(std::ostream &os, const glm::vec<2, T> &v)
    {
        os << "[" << v.x << "," << v.y << "]";
        return os;
    }

} // namespace platinum
#endif