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

#include <camera/perspective_camera.h>

namespace platinum
{

    PerspectiveCamera::PerspectiveCamera(const Transform &camera2world, float fov, std::shared_ptr<Film> film) : ProjectiveCamera(camera2world, Perspective(fov, 1e-2f, 1000.f), film)
    {
        Initialize();
    }
    void PerspectiveCamera::Initialize()
    {
        auto x = _film->GetWidth();
        auto y = _film->GetHeight();
        auto p_min = _raster2camera.ExecOn(glm::vec3(0.f), 1.f);
        auto p_max = _raster2camera.ExecOn(glm::vec3(x, y, 0.f), 1.f);
        p_min /= p_min.z;
        p_max /= p_max.z;
        _area = glm::abs((p_max.x - p_min.x) * (p_max.y - p_min.y));

        ProjectiveCamera::Initialize();
    }

    float PerspectiveCamera::CastRay(const CameraSample &sample, Ray &ray) const
    {
        //取film上一点
        glm::vec3 p_film(sample.p_film.x, sample.p_film.y, 0);
        //转化为相机坐标
        glm::vec3 p_camera = _raster2camera.ExecOn(p_film, 1.f);
        ray = Ray(glm::vec3(0.f), glm::normalize(p_camera));
        ray = _camera2world.ExecOn(ray);
        return 1.f;
    }
}
