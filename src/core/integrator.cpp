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
#include <tbb/tbb.h>
#include <core/memory.h>

namespace platinum
{
    void SamplerIntegrator::Render(const Scene &scene)
    {
        Vector2i resolution = _camera->_film->GetResolution();

        auto &sampler = _sampler;
        auto &arena = MemoryArena::GetInstance();
        // Compute number of tiles, _nTiles_, to use for parallel rendering
        Bounds2i sampleBounds = _camera->_film->GetSampleBounds();
        Vector2i sampleExtent = sampleBounds.Diagonal();
        constexpr int tileSize = 16;
        Vector2i nTiles((sampleExtent.x + tileSize - 1) / tileSize, (sampleExtent.y + tileSize - 1) / tileSize);

        // AReporter reporter(nTiles.x * nTiles.y, "Rendering");
        tbb::parallel_for(tbb::blocked_range<size_t>(0, nTiles.x * nTiles.y),
                          [&](tbb::blocked_range<size_t> r)
                          {
                              for (size_t t = r.begin(); t != r.end(); ++t)
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

                                          arena.Reset();
                                      } while (tileSampler->StartNextSample());
                                  }
                                  LOG(INFO) << "Finished image tile " << tileBounds;

                                  _camera->_film->MergeFilmTile(std::move(filmTile));
                              }
                          });

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
    // // Integrator Utility Functions
    // Spectrum UniformSampleAllLights(const Interaction &it, const Scene &scene,
    //                                Sampler &sampler,
    //                                 const std::vector<int> &nLightSamples,
    //                                 bool handleMedia)
    // {
    //     Spectrum L(0.f);
    //     for (size_t j = 0; j < scene._lights.size(); ++j)
    //     {
    //         // Accumulate contribution of _j_th light to _L_
    //         const std::shared_ptr<Light> &light = scene._lights[j];
    //         int nSamples = nLightSamples[j];
    //         const Vector2f *uLightArray = sampler.Get2DArray(nSamples);
    //         const Vector2f *uScatteringArray = sampler.Get2DArray(nSamples);
    //         if (!uLightArray || !uScatteringArray)
    //         {
    //             // Use a single sample for illumination from _light_
    //             Vector2f uLight = sampler.Get2D();
    //             Vector2f uScattering = sampler.Get2D();
    //             L += EstimateDirect(it, uScattering, *light, uLight, scene, sampler,
    //                                 arena, handleMedia);
    //         }
    //         else
    //         {
    //             // Estimate direct lighting using sample arrays
    //             Spectrum Ld(0.f);
    //             for (int k = 0; k < nSamples; ++k)
    //                 Ld += EstimateDirect(it, uScatteringArray[k], *light,
    //                                      uLightArray[k], scene, sampler, arena,
    //                                      handleMedia);
    //             L += Ld / nSamples;
    //         }
    //     }
    //     return L;
    // }

    // Spectrum UniformSampleOneLight(const Interaction &it, const Scene &scene,
    //                                Sampler &sampler,
    //                                bool handleMedia, const Distribution1D *lightDistrib)
    // {
      
    //     // Randomly choose a single light to sample, _light_
    //     int nLights = int(scene.lights.size());
    //     if (nLights == 0)
    //         return Spectrum(0.f);
    //     int lightNum;
    //     Float lightPdf;
    //     if (lightDistrib)
    //     {
    //         lightNum = lightDistrib->SampleDiscrete(sampler.Get1D(), &lightPdf);
    //         if (lightPdf == 0)
    //             return Spectrum(0.f);
    //     }
    //     else
    //     {
    //         lightNum = std::min((int)(sampler.Get1D() * nLights), nLights - 1);
    //         lightPdf = Float(1) / nLights;
    //     }
    //     const std::shared_ptr<Light> &light = scene.lights[lightNum];
    //     Vector2f uLight = sampler.Get2D();
    //     Vector2f uScattering = sampler.Get2D();
    //     return EstimateDirect(it, uScattering, *light, uLight,
    //                           scene, sampler, handleMedia) /
    //            lightPdf;
    // }

    // Spectrum EstimateDirect(const Interaction &it, const Vector2f &uScattering,
    //                         const Light &light, const Vector2f &uLight,
    //                         const Scene &scene, Sampler &sampler,
    //                         MemoryArena &arena, bool handleMedia, bool specular)
    // {
    //     BxDFType bsdfFlags =
    //         specular ? BSDF_ALL : BxDFType(BSDF_ALL & ~BSDF_SPECULAR);
    //     Spectrum Ld(0.f);
    //     // Sample light source with multiple importance sampling
    //     Vector3f wi;
    //     Float lightPdf = 0, scatteringPdf = 0;
    //     VisibilityTester visibility;
    //     Spectrum Li = light.Sample_Li(it, uLight, &wi, &lightPdf, &visibility);
    //     VLOG(2) << "EstimateDirect uLight:" << uLight << " -> Li: " << Li << ", wi: "
    //             << wi << ", pdf: " << lightPdf;
    //     if (lightPdf > 0 && !Li.IsBlack())
    //     {
    //         // Compute BSDF or phase function's value for light sample
    //         Spectrum f;
    //         if (it.IsSurfaceInteraction())
    //         {
    //             // Evaluate BSDF for light sampling strategy
    //             const SurfaceInteraction &isect = (const SurfaceInteraction &)it;
    //             f = isect.bsdf->f(isect.wo, wi, bsdfFlags) *
    //                 AbsDot(wi, isect.shading.n);
    //             scatteringPdf = isect.bsdf->Pdf(isect.wo, wi, bsdfFlags);
    //             VLOG(2) << "  surf f*dot :" << f << ", scatteringPdf: " << scatteringPdf;
    //         }
    //         else
    //         {
    //             // Evaluate phase function for light sampling strategy
    //             const MediumInteraction &mi = (const MediumInteraction &)it;
    //             Float p = mi.phase->p(mi.wo, wi);
    //             f = Spectrum(p);
    //             scatteringPdf = p;
    //             VLOG(2) << "  medium p: " << p;
    //         }
    //         if (!f.IsBlack())
    //         {
    //             // Compute effect of visibility for light source sample
    //             if (handleMedia)
    //             {
    //                 Li *= visibility.Tr(scene, sampler);
    //                 VLOG(2) << "  after Tr, Li: " << Li;
    //             }
    //             else
    //             {
    //                 if (!visibility.Unoccluded(scene))
    //                 {
    //                     VLOG(2) << "  shadow ray blocked";
    //                     Li = Spectrum(0.f);
    //                 }
    //                 else
    //                     VLOG(2) << "  shadow ray unoccluded";
    //             }

    //             // Add light's contribution to reflected radiance
    //             if (!Li.IsBlack())
    //             {
    //                 if (IsDeltaLight(light.flags))
    //                     Ld += f * Li / lightPdf;
    //                 else
    //                 {
    //                     Float weight =
    //                         PowerHeuristic(1, lightPdf, 1, scatteringPdf);
    //                     Ld += f * Li * weight / lightPdf;
    //                 }
    //             }
    //         }
    //     }

    //     // Sample BSDF with multiple importance sampling
    //     if (!IsDeltaLight(light.flags))
    //     {
    //         Spectrum f;
    //         bool sampledSpecular = false;
    //         if (it.IsSurfaceInteraction())
    //         {
    //             // Sample scattered direction for surface interactions
    //             BxDFType sampledType;
    //             const SurfaceInteraction &isect = (const SurfaceInteraction &)it;
    //             f = isect.bsdf->Sample_f(isect.wo, &wi, uScattering, &scatteringPdf,
    //                                      bsdfFlags, &sampledType);
    //             f *= AbsDot(wi, isect.shading.n);
    //             sampledSpecular = (sampledType & BSDF_SPECULAR) != 0;
    //         }
    //         else
    //         {
    //             // Sample scattered direction for medium interactions
    //             const MediumInteraction &mi = (const MediumInteraction &)it;
    //             Float p = mi.phase->Sample_p(mi.wo, &wi, uScattering);
    //             f = Spectrum(p);
    //             scatteringPdf = p;
    //         }
    //         VLOG(2) << "  BSDF / phase sampling f: " << f << ", scatteringPdf: " << scatteringPdf;
    //         if (!f.IsBlack() && scatteringPdf > 0)
    //         {
    //             // Account for light contributions along sampled direction _wi_
    //             Float weight = 1;
    //             if (!sampledSpecular)
    //             {
    //                 lightPdf = light.Pdf_Li(it, wi);
    //                 if (lightPdf == 0)
    //                     return Ld;
    //                 weight = PowerHeuristic(1, scatteringPdf, 1, lightPdf);
    //             }

    //             // Find intersection and compute transmittance
    //             SurfaceInteraction lightIsect;
    //             Ray ray = it.SpawnRay(wi);
    //             Spectrum Tr(1.f);
    //             bool foundSurfaceInteraction =
    //                 handleMedia ? scene.IntersectTr(ray, sampler, &lightIsect, &Tr)
    //                             : scene.Intersect(ray, &lightIsect);

    //             // Add light contribution from material sampling
    //             Spectrum Li(0.f);
    //             if (foundSurfaceInteraction)
    //             {
    //                 if (lightIsect.primitive->GetAreaLight() == &light)
    //                     Li = lightIsect.Le(-wi);
    //             }
    //             else
    //                 Li = light.Le(ray);
    //             if (!Li.IsBlack())
    //                 Ld += f * Li * Tr * weight / scatteringPdf;
    //         }
    //     }
    //     return Ld;
    // }
}