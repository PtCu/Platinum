

#include <core/camera.h>

namespace platinum
{
    ProjectiveCamera::ProjectiveCamera(const Transform &cameraToWorld, const Transform &cameraToScreen, UPtr<Film> film)
        : Camera(cameraToWorld, std::move(film)), _camera2screen(cameraToScreen)
    {
        ProjectiveCamera::Initialize();
    }
    void ProjectiveCamera::Initialize()
    {
        Bounds2f screen;
        auto res = _film->GetResolution();
        float frame = (float)(res.x) / res.y;
        if (frame > 1.f)
        {
            screen._p_min.x = -frame;
            screen._p_max.x = frame;
            screen._p_min.y = -1.f;
            screen._p_max.y = 1.f;
        }
        else
        {
            screen._p_min.x = -1.f;
            screen._p_max.x = 1.f;
            screen._p_min.y = -1.f / frame;
            screen._p_max.y = 1.f / frame;
        }

        _screen2raster = Scale(res.x, res.y, 1) *
                         Scale(1 / (screen._p_max.x - screen._p_min.x),
                               1 / (screen._p_min.y - screen._p_max.y), 1) *
                         Translate(Vector3f(-screen._p_min.x, -screen._p_max.y, 0));
        _raster2screen = Inverse(_screen2raster);
        _raster2camera = Inverse(_camera2screen) * _raster2screen;
    }
}
