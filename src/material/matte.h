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

#ifndef MATERIAL_MATTE_H_
#define MATERIAL_MATTE_H_

#include <core/material.h>
#include <material/bxdf/lambertian.h>
#include <core/bsdf.h>
#include <core/spectrum.h>
namespace platinum
{
    class Matte final : public Material
    {
    public:
        Matte();
        
        Matte(const Spectrum &r);

        Matte(const PropertyTree &node);

        virtual void ComputeScatteringFunctions(SurfaceInteraction &si,MemoryArena &arena) const override;

        std::string ToString() const { return "Matte"; }

    private:
        Spectrum _Kr;

    };
}

#endif