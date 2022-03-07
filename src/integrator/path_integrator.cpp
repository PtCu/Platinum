

#include "path_integrator.h"
#include <core/bsdf.h>

namespace platinum
{

    REGISTER_CLASS(PathIntegrator, "Path");

    PathIntegrator::PathIntegrator(const PropertyTree &root)
        : SamplerIntegrator(nullptr, nullptr), _max_depth(root.Get<int>("Depth")), _rr_threshold(root.Get<float>("RR", 0.8f))
    {
        _sampler = UPtr<Sampler>(static_cast<Sampler *>(ObjectFactory::CreateInstance(root.Get<std::string>("Sampler.Type"), root.GetChild("Sampler"))));

        _camera = UPtr<Camera>(static_cast<Camera *>(ObjectFactory::CreateInstance(root.Get<std::string>("Camera.Type"), root.GetChild("Camera"))));

        _light_sample_strategy = root.Get<std::string>("Strategy", "spatial");
    }
    void PathIntegrator::Preprocess(const Scene &scene, Sampler &sampler)
    {
        _light_distribution = CreateLightSampleDistribution(_light_sample_strategy, scene);
    }

    Spectrum PathIntegrator::Li(const Scene &scene, const Ray &r, Sampler &sampler, MemoryArena &arena, int depth) const
    {
        // beta为吞吐量，表示路径中光源发出的辐射亮度沿着该路径传递到摄像机的分量：
        Spectrum L(0.f), beta(1.f);
        Ray ray(r);

        bool specular_bounce = false;
        int bounces;
        float eta_scale = 1.f;

        for (bounces = 0;; ++bounces)
        {
            // 如果当前ray是直接从相机发射，
            // 判断光线是否与场景几何图元相交
            // 每次弹射，都计算
            // 1. 直接光照（向光源进行采样），
            // 2. 周围物体的光照（根据材质随机发出一条光线）

            // Intersect ray with scene and store intersection in isect
            SurfaceInteraction isect;
            bool hit = scene.Hit(ray, isect);
            // Possibly add emitted light at intersection
            if (bounces == 0 || specular_bounce)
            { // Add emitted light at path vertex or from the environment
                if (hit)
                {
                    L += beta * isect.Le(-ray._direction);
                }
                else
                {
                    for (const auto &light : scene._infinite_lights)
                        L += beta * light->Le(ray);
                }
            }
            // Terminate path if ray escaped or maxDepth was reached
            if (!hit || bounces >= _max_depth)
                break;

            // Compute scattering functions and skip over medium boundaries
            isect.ComputeScatteringFunctions(ray, arena);

            if (!isect._bsdf)
            {
                ray = isect.SpawnRay(ray._direction);
                --bounces;
                continue;
            }
            const Distribution1D *distrib = _light_distribution->Lookup(isect.p);
            // Sample illumination from lights to find path contribution
            if (isect._bsdf->NumComponents(BxDFType((int)BxDFType::BSDF_ALL & ~(int)BxDFType::BSDF_SPECULAR)) > 0)
            {
                Spectrum Ld = beta * UniformSampleOneLight(isect, scene, arena, sampler, distrib);
                CHECK_GE(Ld.y(), 0.f);
                L += Ld;
            }

            // Sample BSDF to get new path direction
            Vector3f wo = -ray._direction, wi;
            float pdf;
            BxDFType flags;
            Spectrum f = isect._bsdf->SampleF(wo, wi, sampler.Get2D(), pdf, flags, BxDFType::BSDF_ALL);

            if (f.isBlack() || pdf == 0.f)
                break;

            beta *= f * glm::abs(glm::dot(wi, isect.n)) / pdf;
            CHECK_GE(beta.y(), 0.f);
            DCHECK(!glm::isinf(beta.y()));

            specular_bounce = ((int)flags & (int)BxDFType::BSDF_SPECULAR) != 0;
            if (((int)flags & (int)BxDFType::BSDF_SPECULAR) && (int)flags & (int)BxDFType::BSDF_TRANSMISSION)
            {
                float eta = isect._bsdf->_eta;

                eta_scale *= (glm::dot(wo, isect.n) > 0) ? (eta * eta) : 1 / (eta * eta);
            }
            ray = isect.SpawnRay(wi);

            // 为何不直接使用throughput，包含的是radiance，radiance是经过折射缩放的
            // 但rrThroughput没有经过折射缩放，包含的是power，我们需要根据能量去筛选路径
            Spectrum rrBeta = beta * eta_scale;
            if (rrBeta.maxComponentValue() < _rr_threshold && bounces > 3)
            {
                float q = glm::max(0.5f, 1 - rrBeta.maxComponentValue());
                if (sampler.Get1D() < q)
                    break;
                beta /= 1 - q;
                DCHECK(!glm::isinf(beta.y()));
            }
        }
        return L;
    }
}