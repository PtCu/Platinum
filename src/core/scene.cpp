

#include <core/scene.h>

namespace platinum
{
    Scene::Scene(UPtr<Aggregate> aggre, const std::vector<Ptr<Light>> &lights)
        : _aggres(std::move(aggre)), _lights(std::move(lights))
    {
        _worldbound = _aggres->WorldBound();
        for (const auto &light : _lights)
        {
            light->Preprocess(*this);
            if (light->_flags & (int)LightFlags::LightInfinite)
                _infinite_lights.push_back(light);
        }
    }

    void Scene::Initialize()
    {
        _aggres->Initialize();
        _worldbound = _aggres->WorldBound();
        for (const auto &light : _lights)
        {
            light->Preprocess(*this);
            if (light->_flags & (int)LightFlags::LightInfinite)
                _infinite_lights.push_back(light);
        }
    }

    bool Scene::Hit(const Ray &ray, SurfaceInteraction &inter) const
    {
        return _aggres->Hit(ray, inter);
    }
    bool Scene::Hit(const Ray &ray) const
    {
        return _aggres->Hit(ray);
    }
} // namespace platinum
