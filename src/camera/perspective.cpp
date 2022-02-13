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

#include <camera/perspective.h>

namespace platinum
{
    REGISTER_CLASS(PerspectiveCamera, "Perspective");

    PerspectiveCamera::PerspectiveCamera(const PropertyNode &root)
    {
        float _fov = root.get<float>("FOV");

        Vector3f _eye, _focus, _up;
        auto eye_iter = root.get_child("Eye").begin();
        auto focus_iter = root.get_child("Focus").begin();
        auto up_iter = root.get_child("WorldUp").begin();
        for (size_t i = 0; i < 3; ++i, ++eye_iter, ++focus_iter, ++up_iter)
        {
            _eye[i] = eye_iter->second.get_value<float>();
            _focus[i] = focus_iter->second.get_value<float>();
            _up[i] = up_iter->second.get_value<float>();
        }

        _camera2world = Inverse(LookAt(_eye, _focus, _up));
        _camera2screen = Perspective(_fov, 1e-2f, 1000.f);

        _film = UPtr<Film>(static_cast<Film *>(ObjectFactory::CreateInstance(root.get<std::string>("Film.Type"), root.get_child("Film"))));

        Initialize();
    }

    PerspectiveCamera::PerspectiveCamera(const Transform &camera2world, float fov, UPtr<Film> film)
        : ProjectiveCamera(camera2world, Perspective(fov, 1e-2f, 1000.f), std::move(film))
    {
        Initialize();
    }
    void PerspectiveCamera::Initialize()
    {
        auto res = _film->GetResolution();
        auto p_min = _raster2camera.ExecOn(Vector3f(0.f), 1.f);
        auto p_max = _raster2camera.ExecOn(Vector3f(res.x, res.y, 0.f), 1.f);
        p_min /= p_min.z;
        p_max /= p_max.z;
        _area = glm::abs((p_max.x - p_min.x) * (p_max.y - p_min.y));
    }

    float PerspectiveCamera::CastingRay(const CameraSample &sample, Ray &ray) const
    {
        //取film上一点
        Vector3f p_film(sample.p_film.x, sample.p_film.y, 0);
        //转化为相机坐标
        Vector3f p_camera = _raster2camera.ExecOn(p_film, 1.f);
        ray = Ray(Vector3f(0.f), glm::normalize(p_camera));
        ray = _camera2world.ExecOn(ray);
        return 1.f;
    }
}
