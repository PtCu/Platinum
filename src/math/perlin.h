

#ifndef MATH_PERLIN_H_
#define MATH_PERLIN_H_

#include <glm/glm.hpp>
#include <math/rand.h>
#include <chrono> // std::chrono::system_clock

namespace platinum
{
    class Perlin
    {
    public:
        static float Turb(const glm::vec3 &p, size_t depth = 7);

    private:
        static float GenNoise(const glm::vec3 &p);
        static float PerlinInterp(const glm::vec3 c[2][2][2], float u, float v, float w);
        static std::vector<size_t> GenPermute(size_t n);
        static std::vector<glm::vec3> GenRandVec(size_t n);

        static std::vector<glm::vec3> rand_vec_;

        // 0, 1, ... , 255 变更顺序后的序列
        static std::vector<size_t> perm_x_;
        static std::vector<size_t> perm_y_;
        static std::vector<size_t> perm_z_;
    };
}

#endif
