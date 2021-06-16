#include "light.h"

#include "random.h"
#include "scene.h"
#include "sampler.h"

namespace platinum
{

    //-------------------------------------------Light-------------------------------------

    Light::Light(const PropertyList &props)
    {
        m_nSamples = props.getInteger("LightSamples", 1);
    }

    Light::Light(int flags, const Transform &lightToWorld, int nSamples)
        : m_flags(flags), m_nSamples(glm::max(1, nSamples)), m_lightToWorld(lightToWorld),
          m_worldToLight(inverse(lightToWorld))
    {
        //++numLights;
    }

    Light::~Light() {}

    Spectrum Light::Le(const Ray &ray) const { return Spectrum(0.f); }

    //-------------------------------------------VisibilityTester-------------------------------------

    bool VisibilityTester::unoccluded(const Scene &scene) const
    {
        return !scene.hit(m_p0.spawnRayTo(m_p1));
    }

    Spectrum VisibilityTester::tr(const Scene &scene, Sampler &sampler) const
    {
        //Ray ray(p0.SpawnRayTo(p1));
        Spectrum Tr(1.f);
        //while (true) {
        //	SurfaceInteraction isect;
        //	bool hitSurface = scene.Intersect(ray, &isect);
        //	// Handle opaque surface along ray's path
        //	if (hitSurface && isect.primitive->GetMaterial() != nullptr)
        //		return Spectrum(0.0f);

        //	// Update transmittance for current ray segment
        //	if (ray.medium) Tr *= ray.medium->Tr(ray, sampler);

        //	// Generate next ray segment or return final transmittance
        //	if (!hitSurface) break;
        //	ray = isect.SpawnRayTo(p1);
        //}
        return Tr;
    }

    //-------------------------------------------AreaLight-------------------------------------

    AreaLight::AreaLight(const PropertyList &props) : Light(props) { m_flags = (int)LightFlags::Area; }

    AreaLight::AreaLight(const Transform &lightToWorld, int nSamples)
        : Light((int)LightFlags::Area, lightToWorld, nSamples) {}

}