#ifndef CORE_CAMERA_H_
#define CORE_CAMERA_H_

#include "platinum.h"
#include "film.h"
#include "transform.h"
#include "rtti.h"

namespace platinum
{

    struct CameraSample
    {
        Vector2f pFilm;
    };

    inline std::ostream &operator<<(std::ostream &os, const CameraSample &cs)
    {
        os << "[ pFilm: " << cs.pFilm << " ]";
        return os;
    }

    class Camera : public Object
    {
    public:
        typedef std::shared_ptr<Camera> ptr;

        // Camera Interface
        Camera() = default;
        Camera(const Transform &cameraToWorld, Film::ptr film);
        virtual ~Camera();

        virtual Float castingRay(const CameraSample &sample, Ray &ray) const = 0;

        //virtual spectrum We(const Ray &ray, APoint2f *pRaster2 = nullptr) const;
        //virtual void pdf_We(const Ray &ray, Float *pdfPos, Float *pdfDir) const;

        //virtual Spectrum Sample_Wi(const Interaction &ref, const APoint2f &u,
        //	Vector3f *wi, Float *pdf, APoint2f *pRaster, VisibilityTester *vis) const;

        virtual ClassType getClassType() const override { return ClassType::Camera; }

        // Camera Public Data
        Transform m_cameraToWorld;
        Film::ptr m_film = nullptr;
    };

    class ProjectiveCamera : public Camera
    {
    public:
        typedef std::shared_ptr<ProjectiveCamera> ptr;

        ProjectiveCamera() = default;
        ProjectiveCamera(const Transform &cameraToWorld, const Transform &cameraToScreen, Film::ptr film)
            : Camera(cameraToWorld, film), m_cameraToScreen(cameraToScreen) {}

    protected:
        virtual void initialize();

    protected:
        Transform m_cameraToScreen, m_rasterToCamera;
        Transform m_screenToRaster, m_rasterToScreen;
    };

}

#endif