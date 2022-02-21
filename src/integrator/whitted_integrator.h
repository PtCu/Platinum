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

#ifndef INTEGRATOR_WHITTED_INTEGRATOR_H_
#define INTEGRATOR_WHITTED_INTEGRATOR_H_

#include <core/integrator.h>
#include <core/bsdf.h>
namespace platinum
{

    class WhittedIntegrator final: public SamplerIntegrator
    {
    public:
        WhittedIntegrator(const PropertyTree &node);
        WhittedIntegrator(UPtr<Camera> camera, UPtr<Sampler> sampler, int max_depth)
            : SamplerIntegrator(std::move(camera), std::move(sampler)), _max_depth(max_depth) {}
        std::string ToString() const { return "WhittedIntegrator"; }
      virtual void Preprocess(const Scene &scene, Sampler &sampler) override {}

    protected:
        virtual Spectrum Li(const Scene &scene, const Ray &ray, Sampler &sampler, MemoryArena &arena,  int depth) const override;

    private:
        const int _max_depth;
    };
}

#endif