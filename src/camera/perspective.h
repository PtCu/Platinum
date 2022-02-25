

#ifndef CAMERA_PERSPECTIVE_CAMERA_H_
#define CAMERA_PERSPECTIVE_CAMERA_H_

#include <core/camera.h>
#include <math/ray.h>
namespace platinum
{

    class PerspectiveCamera final : public ProjectiveCamera
    {
    public:
        PerspectiveCamera(const PropertyTree &node);

        PerspectiveCamera(const Transform &camera2world, float fov, UPtr<Film> film);

        virtual float CastingRay(const CameraSample &sample, Ray &ray) const override;

        virtual std::string ToString() const { return "PerspectiveCamera"; }

    protected:
        virtual void Initialize() override;

    private:
        //z=1时的film面积
        float _area;
    };
}

#endif