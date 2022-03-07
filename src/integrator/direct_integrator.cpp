#include <integrator/direct_integrator.h>

namespace platinum
{
    REGISTER_CLASS(DirectIntegrator, "Direct");

    DirectIntegrator::DirectIntegrator(const PropertyTree &root)
        : SamplerIntegrator(nullptr, nullptr), _max_depth(root.Get<int>("Depth"))
    {
        _sampler = UPtr<Sampler>(static_cast<Sampler *>(ObjectFactory::CreateInstance(root.Get<std::string>("Sampler.Type"), root.GetChild("Sampler"))));

        _camera = UPtr<Camera>(static_cast<Camera *>(ObjectFactory::CreateInstance(root.Get<std::string>("Camera.Type"), root.GetChild("Camera"))));

        _strategy = (LightStrategy)root.Get<int>("Strategy", 0);
    }

    void DirectIntegrator::Preprocess(const Scene &scene, Sampler &sampler)
    {
        if (_strategy == LightStrategy::UniformSampleAll)
        {
            for (const auto &light : scene._lights)
            {
                _n_light_samples.push_back(sampler.RoundCount(light->_num_samples));
            }
            for (int i = 0; i < _max_depth; ++i)
            {
                for (size_t j = 0; j < scene._lights.size(); ++j)
                {
                    sampler.Request2DArray(_n_light_samples[j]);
                    sampler.Request2DArray(_n_light_samples[j]);
                }
            }
        }
    }
    Spectrum DirectIntegrator::Li(const Scene &scene, const Ray &ray, Sampler &sampler, MemoryArena &arena, int depth) const
    {
        Spectrum L(0.f);
        // Find closest ray intersection or return background radiance
        SurfaceInteraction isect;
        if (!scene.Hit(ray, isect))
        {
            //返回lights emission
            for (const auto &light : scene._lights)
                L += light->Le(ray);
        }
        isect.ComputeScatteringFunctions(ray, arena);
        // 没有bsdf
        if (!isect._bsdf)
            return Li(scene, isect.SpawnRay(ray._direction), sampler, arena, depth);
        Vector3f wo = isect.wo;

        // 如果光线打到光源，计算其发光值 -> Le (emission term)
        L += isect.Le(wo);
        if (scene._lights.size() > 0)
        {
            if (_strategy == LightStrategy::UniformSampleAll)
                //累计所有光源的直接光照值
                L += UniformSampleAllLights(isect, scene, arena, sampler, _n_light_samples);

            else
                L += UniformSampleOneLight(isect, scene, arena, sampler);
        }
        if (depth + 1 < _max_depth)
        {
            //对反射和透射分别计算
            L += SpecularReflect(ray, isect, scene, sampler, arena, depth);
            L += SpecularTransmit(ray, isect, scene, sampler, arena, depth);
        }
        return L;
    }
}