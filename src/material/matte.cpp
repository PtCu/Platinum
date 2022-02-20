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

#include <material/matte.h>

namespace platinum
{

    REGISTER_CLASS(Matte, "Matte");

    Matte::Matte(const PropertyTree &node)
    {
        Vector3f spectrum = node.Get<Vector3f>("R");
        _Kr = Spectrum::fromRGB(spectrum);
    }

    Matte::Matte() : _Kr(Spectrum(0.73, 0.73, 0.73))
    {
    }

    Matte::Matte(const Spectrum &r) : _Kr(r)
    {
    }

    void Matte::ComputeScatteringFunctions(SurfaceInteraction &si) const
    {
        //每一个Interaction的BSDF都可能不一样，故生命周期由由该Interaction掌管
        si._bsdf = new BSDF{si};
        if (!_Kr.isBlack())
        {
            si._bsdf->Add(new LambertianReflection{_Kr});
        }
    }
}