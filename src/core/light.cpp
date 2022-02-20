// Copyright 2022 ptcup
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

#include <core/light.h>
#include <core/scene.h>
namespace platinum
{
    bool VisibilityTester::Unoccluded(const Scene &scene) const
    {
        return !scene.Hit(_p0.SpawnRayTo(_p1));
    }

    Light::Light(const PropertyTree &node)
    {
        _num_samples = node.Get<int>("LightSamples", 1);
    }
    AreaLight::AreaLight(const PropertyTree &node)
        : Light(node) { _flags = (int)LightFlags::LightArea; }

    std::unique_ptr<LightDistribution> createLightSampleDistribution(
        const std::string &name, const Scene &scene)
    {

        return std::unique_ptr<LightDistribution>{
            new UniformLightDistribution(scene)};

            //TODO:: other distribution type
    }

    UniformLightDistribution::UniformLightDistribution(const Scene &scene)
    {
        std::vector<float> prob(scene._lights.size(), float(1));
        distrib.reset(new Distribution1D(&prob[0], int(prob.size())));
    }

    const Distribution1D *UniformLightDistribution::lookup(const Vector3f &p) const
    {
        return distrib.get();
    }
}