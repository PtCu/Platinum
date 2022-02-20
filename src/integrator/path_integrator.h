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

#ifndef INTEGRATOR_PATH_INTEGRATOR_H_
#define INTEGRATOR_PATH_INTEGRATOR_H_

#include <core/integrator.h>

namespace platinum
{

    class PathIntegrator : public SamplerIntegrator
    {
    public:
        PathIntegrator(const PropertyTree &root);

        PathIntegrator(UPtr<Camera> camera, UPtr<Sampler> sampler, int max_depth, float rr_threshold = 0.8)
            : SamplerIntegrator(std::move(camera), std::move(sampler)), _max_depth(max_depth), _rr_threshold(rr_threshold) {}

        std::string ToString() const { return "PathIntegrator"; }

    protected:
        virtual Spectrum Li(const Scene &scene, const Ray &ray, Sampler &sampler, int depth) const override;
        const int _max_depth;
        float _rr_threshold;
        std::string m_lightSampleStrategy;
        std::unique_ptr<LightDistribution> m_lightDistribution;
    };
}
#endif