

#include <core/light.h>
#include <core/scene.h>
namespace platinum
{
    bool VisibilityTester::Unoccluded(const Scene &scene) const
    {
        return !scene.Hit(_p0.SpawnRayTo(_p1));
    }

    Light::Light(const PropertyTree &node)
    {
        _num_samples = node.Get<int>("LightSamples", 1);
    }
    AreaLight::AreaLight(const PropertyTree &node)
        : Light(node) { _flags = (int)LightFlags::LightArea; }

    std::unique_ptr<LightDistribution> CreateLightSampleDistribution(
        const std::string &name, const Scene &scene)
    {

        return std::unique_ptr<LightDistribution>{
            new UniformLightDistribution(scene)};

        //TODO:: other distribution type
    }

    UniformLightDistribution::UniformLightDistribution(const Scene &scene)
    {
        std::vector<float> prob(scene._lights.size(), float(1));
        distrib.reset(new Distribution1D(&prob[0], int(prob.size())));
    }

    const Distribution1D *UniformLightDistribution::Lookup(const Vector3f &p) const
    {
        return distrib.get();
    }
}