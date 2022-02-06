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

#ifndef CAMERA_PERSPECTIVE_CAMERA_H_
#define CAMERA_PERSPECTIVE_CAMERA_H_

#include <core/camera.h>
#include <core/ray.h>
namespace platinum
{

    class PerspectiveCamera final : public ProjectiveCamera
    {
    public:
        PerspectiveCamera(const Transform &camera2world, float fov, Ptr<Film> film);

        virtual float CastingRay(const CameraSample &sample, Ray &ray) const override;

    protected:
        virtual void Initialize() override;

    private:
        //z=1时的film面积
        float _area;
    };
}

#endif