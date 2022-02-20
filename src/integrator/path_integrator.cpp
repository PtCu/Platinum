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

#include "path_integrator.h"

namespace platinum {

    REGISTER_CLASS(PathIntegrator, "Path");

    PathIntegrator::PathIntegrator(const PropertyTree &root)
        : SamplerIntegrator(nullptr, nullptr), _max_depth(root.Get<int>("Depth")),m_lightSampleStrategy("spatial")
    {
        _sampler = UPtr<Sampler>(static_cast<Sampler *>(ObjectFactory::CreateInstance(root.Get<std::string>("Sampler.Type"), root.GetChild("Sampler"))));

        _camera = UPtr<Camera>(static_cast<Camera *>(ObjectFactory::CreateInstance(root.Get<std::string>("Camera.Type"), root.GetChild("Camera"))));
    }
    Spectrum PathIntegrator::Li(const Scene &scene, const Ray &r, Sampler &sampler, int depth) const
    {
        Spectrum L(0.f), beta(1.f);
        Ray ray(r);

        bool specular_bounce = false;
        int bounces;
        float eta_scale = 1.f;
      
        return Spectrum(0.f);
    }
}