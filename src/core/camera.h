// The MIT License (MIT)

// Copyright (c) 2021 PtCu

//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

#ifndef CORE_CAMERA_H_
#define CORE_CAMERA_H_

#include <math/rand.h>
#include <math/transform.h>
#include <core/film.h>
namespace platinum
{
    struct CameraSample
    {
        glm::vec2 p_film;
    };

    inline std::ostream &operator<<(std::ostream &os, const CameraSample &cs)
    {
        os << "[ pFilm: " << cs.p_film << " ]";
        return os;
    }
    class Camera
    {
    public:
        Camera() = default;
        Camera(const Transform& camera2world, std::shared_ptr<Film> film)
            :_camera2world(camera2world), _film(film) {}
        virtual ~Camera()=default;
        /**
         * @brief 
         * @param  sample           My Param doc
         * @param  ray              My Param doc
         * @return float            radiance
         */

        void SetFilm(std::shared_ptr<Film> film) { _film = film; }
        std::shared_ptr<Film> GetFilm() { return _film; }

        virtual float CastingRay(const CameraSample& sample, Ray& ray)const = 0;

        Transform _camera2world;
        std::shared_ptr<Film> _film;

    };

    /**
     * @brief pbrt用的左手系，朝向z轴正方向
     */
    class ProjectiveCamera : public Camera
    {
    public:
        ProjectiveCamera() = default;
        ProjectiveCamera(const Transform& cameraToWorld, const Transform& cameraToScreen, std::shared_ptr<Film> film)
            : Camera(cameraToWorld, film), _camera2screen(cameraToScreen) { }

    protected:
        /**
         * @brief   初始化_screen2raster, _raster2screen,_raster2camera.
         *          必须由基类初始化完_camera2screen才能调用此函数
         */
        virtual void Initialize();

    protected:
        Transform _camera2screen, _raster2camera;
        Transform _screen2raster, _raster2screen;
    };

} // namespace platinum

#endif
