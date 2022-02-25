

#ifndef CORE_RAND_H_
#define CORE_RAND_H_

#include <random>
#include <glm/glm.hpp>

namespace platinum
{
    class Rng
    {
    public:
        static float UniformFloat()
        {
            static std::random_device seed_gen;
            static std::mt19937 engine(seed_gen());
            static std::uniform_real_distribution<> dist(0.f, 1.f);
            return static_cast<float>(dist(engine));
        }
        static glm::vec2 UniformDisk()
        {
            glm::vec2 p;
            do
            {
                p = 2.0f * glm::vec2(UniformFloat(), UniformFloat()) - glm::vec2(1, 1);
            } while (glm::dot(p, p) >= 1.0);
            return p;
        }
        static glm::vec3 UniformSphere()
        {
            glm::vec3 p;
            do
            {
                p = 2.0f * glm::vec3(UniformFloat(), UniformFloat(), UniformFloat()) - glm::vec3(1, 1, 1);
            } while (glm::dot(p, p) >= 1.0);
            return p;
        }
    };

} // namespace platinum

#endif
