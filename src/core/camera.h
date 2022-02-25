

#ifndef CORE_CAMERA_H_
#define CORE_CAMERA_H_

#include <math/rand.h>
#include <math/transform.h>
#include <core/film.h>
#include <core/object.h>
namespace platinum
{
    struct CameraSample
    {
        Vector2f p_film;
    };

    inline std::ostream &operator<<(std::ostream &os, const CameraSample &cs)
    {
        os << "[ pFilm: " << cs.p_film << " ]";
        return os;
    }
    class Camera : public Object
    {
    public:
        Camera() = default;

        Camera(const Transform &camera2world, UPtr<Film> film)
            : _camera2world(camera2world), _film(std::move(film)) {}

        void SetFilm(UPtr<Film> film)
        {
            _film = std::move(film);
        }

        /**
         * @brief 按照相机上的采样生成一束光线
         * 
         * @return float  How much the radiance arriving at the film plane along the generated ray will contribute to the final image.
         */
        virtual float CastingRay(const CameraSample &sample, Ray &ray) const = 0;

        Transform _camera2world;
        UPtr<Film> _film;
    };

    /**
     * @brief pbrt用的左手系，朝向z轴正方向
     */
    class ProjectiveCamera : public Camera
    {
    public:
        ProjectiveCamera() = default;

        ProjectiveCamera(const Transform &cameraToWorld, const Transform &cameraToScreen, UPtr<Film> film);

    protected:
        /**
         * @brief   用cameraToWorld和cameraToScreen初始化_screen2raster, _raster2screen,_raster2camera.
         *                  
         * */
        virtual void Initialize();

    protected:
        Transform _camera2screen, _raster2camera;
        Transform _screen2raster, _raster2screen;
    };

} // namespace platinum

#endif
