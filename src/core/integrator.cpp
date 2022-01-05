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
#include "integrator.h"

namespace platinum
{
    glm::vec3 Integrator::SpecularReflect(const Ray& ray, const Interaction& inter, const Scene& scene, Sampler& sampler, int depth)const {
        return glm::vec3(0);

    }
    glm::vec3 Integrator::SpecularTransmit(const Ray& ray, const Interaction& inter, const Scene& scene, Sampler& sampler, int depth)const {
        return glm::vec3(0);

    }

    void TiledIntegrator::UpdateProgress(float progress)
    {
        int barWidth = 70;

        std::cout << "[";
        int pos = static_cast<int>(barWidth * progress);
        for (int i = 0; i < barWidth; ++i)
        {
            if (i < pos)
                std::cout << "=";
            else if (i == pos)
                std::cout << ">";
            else
                std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << " %\r";
        std::cout.flush();
    };

    void TiledIntegrator::Render(const Scene& scene)
    {
        auto film = this->_camera->GetFilm();
        int width = film->GetWidth();
        int height = film->GetHeight();
        int img_size = film->GetImageSize();
        int tiles_count = _tiles_manager->GetTilesCount();
        int has_finished_num = 0;
        auto sampler = _sampler;
        for (int k = 0;k < _spp;++k) {
            auto calculateRstForEachTile = [&](int rank)
            {
                const RenderTile& tile = _tiles_manager->GetTile(rank);
                std::unique_ptr<Sampler>tile_sampler(sampler->Clone(k * tiles_count + rank));
                for (size_t i = tile.min_x; i < tile.max_x; ++i)
                {
                    for (size_t j = tile.min_y; j < tile.max_y; ++j)
                    {
                        glm::ivec2 pixel{ i,j };
                        tile_sampler->StartPixel(pixel);
                        CameraSample cam_sample;
                        tile_sampler->GetCameraSample(pixel);
                        int px_id = j * width + i;
                        float u = static_cast<float>(i + Random::UniformFloat()) / static_cast<float>(width);
                        float v = static_cast<float>(j + Random::UniformFloat()) / static_cast<float>(height);
                        auto r = _camera->GetRay(u, v);
                        auto rst = Li(scene, r, *tile_sampler);
                        film->AddPixelValue(px_id, rst / static_cast<float>(_spp));

                        //++has_finished_num;
                        //++px_id;
                    }
                    // 互斥锁，用于打印处理进程（加上之后影响速度，变为顺序执行）
                    // std::lock_guard<std::mutex> g1(_mutex_ins);
                    // UpdateProgress(static_cast<float>(has_finished_num) / img_size);
                }
            };
            //concurrency::parallel_for in ppl.h is windows only, so here I just choose thread function.
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

}