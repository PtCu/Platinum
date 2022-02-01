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
        _tiles_manager = std::make_unique<TilesManager>(_camera->GetFilm()->getResolution().x, _camera->GetFilm()->getResolution().y);
    }

    Spectrum TiledIntegrator::SpecularReflect(const Ray &ray, const SurfaceInteraction &inter,
                                              const Scene &scene, Sampler &sampler, int depth) const
    {
        // Compute specular reflection direction _wi_ and BSDF value
        glm::vec3 wo = inter.wo, wi;
        float pdf;
        BxDFType sampledType;
        BxDFType type = BxDFType(static_cast<int>(BxDFType::BSDF_REFLECTION) | static_cast<int>(BxDFType::BSDF_SPECULAR));

        Spectrum f = inter.bsdf->SampleF(wo, wi, sampler.Get2D(), pdf, sampledType, type);

        // Return contribution of specular reflection
        const glm::vec3 &ns = inter.n;

        if (pdf > 0.f && !f.isBlack() && glm::abs(glm::dot(wi, ns)) != 0.f)
        {
            // Compute ray differential _rd_ for specular reflection
            Ray rd = inter.SpawnRay(wi);
            return f * Li(scene, rd, sampler, depth + 1) * glm::abs(glm::dot(wi, ns)) / pdf;
        }
        else
        {
            return Spectrum(0.f);
        }
    }
    Spectrum TiledIntegrator::SpecularTransmit(const Ray &ray, const SurfaceInteraction &inter,
                                               const Scene &scene, Sampler &sampler, int depth) const
    {
        glm::vec3 wo = inter.wo, wi;
        float pdf;
        const glm::vec3 &p = inter.p;
        const BSDF &bsdf = *(inter.bsdf);
        BxDFType type = BxDFType(static_cast<int>(BxDFType::BSDF_TRANSMISSION) | static_cast<int>(BxDFType::BSDF_SPECULAR));
     
        BxDFType sampledType;
        Spectrum f = bsdf.SampleF(wo, wi, sampler.Get2D(), pdf, sampledType, type);
        Spectrum L = Spectrum(0.f);
        glm::vec3 ns = inter.n;

        if (pdf > 0.f && !f.isBlack() && glm::abs(glm::dot(wi, ns)) != 0.f)
        {
            // Compute ray differential _rd_ for specular transmission
            Ray rd = inter.SpawnRay(wi);
            L = f * Li(scene, rd, sampler, depth + 1) * glm::abs(glm::dot(wi, ns)) / pdf;
        }
        return L;
    }

    void TiledIntegrator::Render(const Scene &scene)
    {
        glm::ivec2 resolution = _camera->_film->getResolution();

        auto &sampler = _sampler;

        // Compute number of tiles, _nTiles_, to use for parallel rendering
        Bounds2i sampleBounds = _camera->_film->getSampleBounds();
        glm::ivec2 sampleExtent = sampleBounds.Diagonal();
        constexpr int tileSize = 16;
        glm::ivec2 nTiles((sampleExtent.x + tileSize - 1) / tileSize, (sampleExtent.y + tileSize - 1) / tileSize);

        // AReporter reporter(nTiles.x * nTiles.y, "Rendering");
        ParallelUtils::parallelFor((size_t)0, (size_t)(nTiles.x * nTiles.y), [&](const size_t &t)
                                   {
                                       glm::ivec2 tile(t % nTiles.x, t / nTiles.x);

                                       // Get sampler instance for tile
                                       int seed = t;
                                       std::unique_ptr<Sampler> tileSampler = sampler->Clone(seed);

                                       // Compute sample bounds for tile
                                       int x0 = sampleBounds._p_min.x + tile.x * tileSize;
                                       int x1 = glm::min(x0 + tileSize, sampleBounds._p_max.x);
                                       int y0 = sampleBounds._p_min.y + tile.y * tileSize;
                                       int y1 = glm::min(y0 + tileSize, sampleBounds._p_max.y);
                                       Bounds2i tileBounds(glm::ivec2(x0, y0), glm::ivec2(x1, y1));
                                       LOG(INFO) << "Starting image tile " << tileBounds;

                                       // Get _FilmTile_ for tile
                                       std::unique_ptr<FilmTile> filmTile = _camera->_film->getFilmTile(tileBounds);

                                       // Loop over pixels in tile to render them
                                       for (glm::ivec2 pixel : tileBounds)
                                       {
                                           tileSampler->StartPixel(pixel);

                                           do
                                           {
                                               // Initialize _CameraSample_ for current sample
                                               CameraSample cameraSample = tileSampler->GetCameraSample(pixel);

                                               // Generate camera ray for current sample
                                               Ray ray;
                                               float rayWeight = _camera->CastingRay(cameraSample, ray);

                                               // Evaluate radiance along camera ray
                                               Spectrum L(0.f);
                                               if (rayWeight > 0)
                                               {
                                                   L = Li(scene, ray, *tileSampler);
                                               }

                                               // Issue warning if unexpected radiance value returned
                                               if (L.hasNaNs())
                                               {
                                                   LOG(ERROR) << stringPrintf(
                                                       "Not-a-number radiance value returned "
                                                       "for pixel (%d, %d), sample %d. Setting to black.",
                                                       pixel.x, pixel.y,
                                                       (int)tileSampler->CurrentSampleIndex());
                                                   L = Spectrum(0.f);
                                               }
                                               else if (L.y() < -1e-5)
                                               {
                                                   LOG(ERROR) << stringPrintf(
                                                       "Negative luminance value, %f, returned "
                                                       "for pixel (%d, %d), sample %d. Setting to black.",
                                                       L.y(), pixel.x, pixel.y,
                                                       (int)tileSampler->CurrentSampleIndex());
                                                   L = Spectrum(0.f);
                                               }
                                               else if (std::isinf(L.y()))
                                               {
                                                   LOG(ERROR) << stringPrintf(
                                                       "Infinite luminance value returned "
                                                       "for pixel (%d, %d), sample %d. Setting to black.",
                                                       pixel.x, pixel.y,
                                                       (int)tileSampler->CurrentSampleIndex());
                                                   L = Spectrum(0.f);
                                               }
                                               VLOG(1) << "Camera sample: " << cameraSample << " -> ray: " << ray << " -> L = " << L;

                                               // Add camera ray's contribution to image
                                               filmTile->addSample(cameraSample.p_film, L, rayWeight);

                                           } while (tileSampler->StartNextSample());
                                       }
                                       LOG(INFO) << "Finished image tile " << tileBounds;

                                       _camera->_film->mergeFilmTile(std::move(filmTile));
                                       //    reporter.update();
                                   },
                                   ExecutionPolicy::SERIAL);

        // reporter.done();

        LOG(INFO) << "Rendering finished";

        _camera->_film->writeImageToFile();
    }

}