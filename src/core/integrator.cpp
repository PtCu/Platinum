// Copyright 2021 ptcup
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <core/integrator.h>
#include <core/bsdf.h>
#include <core/camera.h>
namespace platinum
{
    TiledIntegrator::TiledIntegrator(std::shared_ptr<Camera> camera, std::shared_ptr<Sampler> sampler, int max_depth)
        : _camera(camera), _sampler(sampler), _max_depth(max_depth)
    {
        _tiles_manager = std::make_unique<TilesManager>(_camera->GetFilm()->GetWidth(), _camera->GetFilm()->GetHeight());
    }

    glm::vec3 TiledIntegrator::SpecularReflect(const Ray& ray, const SurfaceInteraction& inter,
        const Scene& scene, Sampler& sampler, int depth) const
    {
        glm::vec3 wo = inter.wo;
        glm::vec3 wi;
        float pdf;
        BxDFType sampledType;
        BxDFType type = BxDFType(static_cast<int>(BxDFType::BSDF_REFLECTION) | static_cast<int>(BxDFType::BSDF_SPECULAR));
        //采样得到入射光
        glm::vec3 f = inter.bsdf->SampleF(wo, wi, sampler.Get2D(), pdf, sampledType, type);

        const glm::vec3& ns = inter.n;
        if (pdf > 0.f && f != glm::vec3(0) && glm::abs(glm::dot(wi, ns)) != 0.f)
        {
            Ray rd = inter.SpawnRay(wi);
            return f * Li(scene, rd, sampler, depth + 1) * glm::abs(glm::dot(wi, ns));
        }
        else
        {
            return glm::vec3(0.f);
        }
    }
    glm::vec3 TiledIntegrator::SpecularTransmit(const Ray& ray, const SurfaceInteraction& inter,
        const Scene& scene, Sampler& sampler, int depth) const
    {
        return glm::vec3(0);
    }



    void TiledIntegrator::Render(const Scene& scene)
    {
        auto film = this->_camera->GetFilm();
        int width = film->GetWidth();
        int height = film->GetHeight();
        int img_size = film->GetImageSize();
        int tiles_count = _tiles_manager->GetTilesCount();
        int has_finished_num = 0;
        auto sampler = _sampler;

        auto calculateRstForEachTile = [&](int rank)
        {
            const RenderTile& tile = _tiles_manager->GetTile(rank);
            std::unique_ptr<Sampler> tile_sampler(sampler->Clone(rank));

            do {
                for (size_t i = tile.min_x; i < tile.max_x; ++i)
                {
                    for (size_t j = tile.min_y; j < tile.max_y; ++j)
                    {
                        glm::ivec2 pixel{ i, j };
                        tile_sampler->StartPixel(pixel);
                        CameraSample cam_sample = tile_sampler->GetCameraSample(pixel);
                        Ray ray;
                        float ray_weight = _camera->CastRay(cam_sample, ray);
                        glm::vec3 L(0.f);
                        if (ray_weight > 0)
                        {
                            L = Li(scene, ray, *tile_sampler);
                        }
                        int px_id = j * width + i;
                        // film->AddPixelValue(px_id, L / static_cast<float>(_spp));


                    }

                }
            } while (tile_sampler->StartNextSample());

        };

        std::vector<std::thread> threads(tiles_count);

        for (int i = 0; i < tiles_count; ++i)
        {
            threads[i] = std::move(std::thread(calculateRstForEachTile, i));
        }
        for (auto& th : threads)
        {
            th.join();
        }
        // UpdateProgress(1.f);
        film->SaveImage();

    }

}