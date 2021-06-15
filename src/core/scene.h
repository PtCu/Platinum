#ifndef CORE_SCENE_H_
#define CORE_SCENE_H_

#include "platinum.h"
#include "math_utils.h"
#include "light.h"
#include "entity.h"
#include "rtti.h"

#include <vector>

namespace platinum
{

    class Scene final
    {
    public:
        typedef std::shared_ptr<Scene> ptr;

        Scene(const std::vector<Entity::ptr> &entities, const HitableAggregate::ptr &aggre,
              const std::vector<Light::ptr> &lights)
            : m_lights(lights), m_aggreShape(aggre), m_entities(entities)
        {
            m_worldBound = m_aggreShape->worldBound();
            for (const auto &light : lights)
            {
                light->preprocess(*this);
                if (light->m_flags & (int)LightFlags::ALightInfinite)
                    m_infiniteLights.push_back(light);
            }
        }

        const Bounds3f &worldBound() const { return m_worldBound; }

        bool hit(const Ray &ray) const;
        bool hit(const Ray &ray, SurfaceInteraction &isect) const;
        bool hitTr(Ray ray, Sampler &sampler, SurfaceInteraction &isect, Spectrum &transmittance) const;

        std::vector<Light::ptr> m_lights;
        // Store infinite light sources separately for cases where we only want
        // to loop over them.
        std::vector<Light::ptr> m_infiniteLights;

    private:
        // Scene Private Data
        Bounds3f m_worldBound;
        HitableAggregate::ptr m_aggreShape;
        std::vector<Entity::ptr> m_entities;
    };

}
#endif