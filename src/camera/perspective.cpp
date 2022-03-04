

#include <camera/perspective.h>

namespace platinum
{
    REGISTER_CLASS(PerspectiveCamera, "Perspective");

    PerspectiveCamera::PerspectiveCamera(const PropertyTree &root)
    {
        float _fov = root.Get<float>("Fov");

        Vector3f _eye, _focus, _up;
        _eye = root.Get<Vector3f>("Eye");
        _focus = root.Get<Vector3f>("Focus");
        _up = root.Get<Vector3f>("WorldUp");

        _camera2world = Inverse(LookAt(_eye, _focus, _up));
        _camera2screen = Perspective(_fov, 1e-2f, 1000.f);

        _film = UPtr<Film>(static_cast<Film *>(ObjectFactory::CreateInstance("Film", root.GetChild("Film"))));

        ProjectiveCamera::Initialize();

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
        //转化为世界坐标
        ray = _camera2world.ExecOn(ray);
        return 1.f;
    }
}
