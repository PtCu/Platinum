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
    void Matte::ComputeScatteringFunctions(SurfaceInteraction &si) const
    {
        //每一个Interaction的BSDF都可能不一样，故生命周期由由该Interaction掌管
        si._bsdf = std::make_shared<BSDF>(si);
        if (!_Kr.isBlack())
        {
            si._bsdf->Add(_ref.get());
        }
    }
}