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
    void SamplerIntegrator::Render(const Scene &scene)
    {
        Vector2i resolution = _camera->_film->GetResolution();

        auto &sampler = _sampler;

        // Compute number of tiles, _nTiles_, to use for parallel rendering
        Bounds2i sampleBounds = _camera->_film->GetSampleBounds();
        Vector2i sampleExtent = sampleBounds.Diagonal();
        constexpr int tileSize = 16;
        Vector2i nTiles((sampleExtent.x + tileSize - 1) / tileSize, (sampleExtent.y + tileSize - 1) / tileSize);

        // AReporter reporter(nTiles.x * nTiles.y, "Rendering");
        ParallelUtils::parallelFor((size_t)0, (size_t)(nTiles.x * nTiles.y), [&](const size_t &t)
                                   {
                                       Vector2i tile(t % nTiles.x, t / nTiles.x);

                                       // Get sampler instance for tile
                                       int seed = t;
                                       std::unique_ptr<Sampler> tileSampler = sampler->Clone(seed);

                                       // Compute sample bounds for tile
                                       int x0 = sampleBounds._p_min.x + tile.x * tileSize;
                                       int x1 = glm::min(x0 + tileSize, sampleBounds._p_max.x);
                                       int y0 = sampleBounds._p_min.y + tile.y * tileSize;
                                       int y1 = glm::min(y0 + tileSize, sampleBounds._p_max.y);
                                       Bounds2i tileBounds(Vector2i(x0, y0), Vector2i(x1, y1));
                                       LOG(INFO) << "Starting image tile " << tileBounds;

                                       // Get _FilmTile_ for tile
                                       std::unique_ptr<FilmTile> filmTile = _camera->_film->GetFilmTile(tileBounds);

                                       // Loop over pixels in tile to render them
                                       for (Vector2i pixel : tileBounds)
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
                                                   LOG(ERROR) << StringPrintf(
                                                       "Not-a-number radiance value returned "
                                                       "for pixel (%d, %d), sample %d. Setting to black.",
                                                       pixel.x, pixel.y,
                                                       (int)tileSampler->CurrentSampleIndex());
                                                   L = Spectrum(0.f);
                                               }
                                               else if (L.y() < -1e-5)
                                               {
                                                   LOG(ERROR) << StringPrintf(
                                                       "Negative luminance value, %f, returned "
                                                       "for pixel (%d, %d), sample %d. Setting to black.",
                                                       L.y(), pixel.x, pixel.y,
                                                       (int)tileSampler->CurrentSampleIndex());
                                                   L = Spectrum(0.f);
                                               }
                                               else if (std::isinf(L.y()))
                                               {
                                                   LOG(ERROR) << StringPrintf(
                                                       "Infinite luminance value returned "
                                                       "for pixel (%d, %d), sample %d. Setting to black.",
                                                       pixel.x, pixel.y,
                                                       (int)tileSampler->CurrentSampleIndex());
                                                   L = Spectrum(0.f);
                                               }
                                               VLOG(1) << "Camera sample: " << cameraSample << " -> ray: " << ray << " -> L = " << L;

                                               // Add camera ray's contribution to image
                                               filmTile->AddSample(cameraSample.p_film, L, rayWeight);

                                           } while (tileSampler->StartNextSample());
                                       }
                                       LOG(INFO) << "Finished image tile " << tileBounds;

                                       _camera->_film->MergeFilmTile(std::move(filmTile));
                                       //    reporter.update();
                                   },
                                   ExecutionPolicy::PARALLEL);

        // reporter.done();

        LOG(INFO) << "Rendering finished";

        _camera->_film->WriteImageToFile();
    }

    Spectrum SamplerIntegrator::SpecularReflect(const Ray &ray, const SurfaceInteraction &inter,
                                                const Scene &scene, Sampler &sampler, int depth) const
    {
        // Compute specular reflection direction _wi_ and BSDF value
        Vector3f wo = inter.wo, wi;
        float pdf;
        BxDFType sampledType;
        BxDFType type = BxDFType(static_cast<int>(BxDFType::BSDF_REFLECTION) | static_cast<int>(BxDFType::BSDF_SPECULAR));

        Spectrum f = inter._bsdf->SampleF(wo, wi, sampler.Get2D(), pdf, sampledType, type);

        // Return contribution of specular reflection
        const Vector3f &ns = inter.n;

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
    Spectrum SamplerIntegrator::SpecularTransmit(const Ray &ray, const SurfaceInteraction &inter,
                                                 const Scene &scene, Sampler &sampler, int depth) const
    {
        Vector3f wo = inter.wo, wi;
        float pdf;
        const Vector3f &p = inter.p;
        const BSDF &bsdf = *(inter._bsdf);
        BxDFType type = BxDFType(static_cast<int>(BxDFType::BSDF_TRANSMISSION) | static_cast<int>(BxDFType::BSDF_SPECULAR));

        BxDFType sampledType;
        Spectrum f = bsdf.SampleF(wo, wi, sampler.Get2D(), pdf, sampledType, type);
        Spectrum L = Spectrum(0.f);
        Vector3f ns = inter.n;

        if (pdf > 0.f && !f.isBlack() && glm::abs(glm::dot(wi, ns)) != 0.f)
        {
            // Compute ray differential _rd_ for specular transmission
            Ray rd = inter.SpawnRay(wi);
            L = f * Li(scene, rd, sampler, depth + 1) * glm::abs(glm::dot(wi, ns)) / pdf;
        }
        return L;
    }

    
}