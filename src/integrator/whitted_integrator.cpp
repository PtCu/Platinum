

#include "whitted_integrator.h"
#include <shape/sphere.h>
namespace platinum
{

    REGISTER_CLASS(WhittedIntegrator, "Whitted");

    WhittedIntegrator::WhittedIntegrator(const PropertyTree &root)
        : SamplerIntegrator(nullptr, nullptr), _max_depth(root.Get<int>("Depth"))
    {
        _sampler = UPtr<Sampler>(static_cast<Sampler *>(ObjectFactory::CreateInstance(root.Get<std::string>("Sampler.Type"), root.GetChild("Sampler"))));

        _camera = UPtr<Camera>(static_cast<Camera *>(ObjectFactory::CreateInstance(root.Get<std::string>("Camera.Type"), root.GetChild("Camera"))));
    }

    //https://pbr-book.org/3ed-2018/Introduction/pbrt_System_Overview#WhittedIntegrator
    Spectrum WhittedIntegrator::Li(const Scene &scene, const Ray &ray, Sampler &sampler, MemoryArena &arena, int depth) const
    {

        Spectrum L{0.f};

        SurfaceInteraction inter;
        // Find closest ray intersection or return background radiance
        if (!scene.Hit(ray, inter))
        {
            //返回lights emission
            for (const auto &light : scene._lights)
                L += light->Le(ray);
            return L;
        }

        const Vector3f &n = inter.n;
        Vector3f wo = inter.wo;

        inter.ComputeScatteringFunctions(ray, arena);
        // 没有bsdf
        if (!inter._bsdf)
        {
            return Li(scene, inter.SpawnRay(ray.GetDirection()), sampler, arena, depth);
        }

        // 如果光线打到光源，计算其发光值 -> Le (emission term)
        L += inter.Le(wo);

        //对每个光源，计算其贡献
        for (const auto &light : scene._lights)
        {
            float pdf;
            Vector3f wi;
            VisibilityTester visibility_tester;
            Spectrum sampled_li = light->SampleLi(inter, sampler.Get2D(), wi, pdf, visibility_tester);

            if (sampled_li.isBlack() || pdf == 0)
                continue;

            Spectrum f = inter._bsdf->F(wo, wi);

            //如果所采样的光源上的光线没被遮挡
            if (!f.isBlack() && visibility_tester.Unoccluded(scene))
            {
                L += f * sampled_li * glm::abs(glm::dot(wi, n)) / pdf;
            }
        }
        if (depth + 1 < _max_depth)
        {
            // Trace rays for specular reflection and refraction
            L += SpecularReflect(ray, inter, scene, sampler, arena, depth);
            L += SpecularTransmit(ray, inter, scene, sampler, arena, depth);
        }
        return L;
    }

}
