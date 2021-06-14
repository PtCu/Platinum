#ifndef CORE_LIGHT_H_
#define CORE_LIGHT_H_

#include "platinum.h"
#include "math/utils.h"
#include "rtti.h"

PLATINUM_BEGIN
enum class LightFlags
{
    // 点光源
    DeltaPosition = 1,
    // 方向光，只有一个方向
    DeltaDirection = 2,
    // 面光源
    Area = 4,
    Infinite = 8,
    // 环境光
    Env = 16
};

inline bool isDeltaLight(int flags)
{
    return flags & (int)ALightFlags::ALightDeltaPosition || flags & (int)ALightFlags::ALightDeltaDirection;
}

class Light : public Object
{
public:
    typedef std::shared_ptr<Light> ptr;

    Light(const PropertyList &props);
    Light(int flags, const Transform &LightToWorld, int nSamples = 1);

    virtual ~Light();

    virtual Spectrum power() const = 0;

    virtual void preprocess(const Scene &scene) {}

    virtual Spectrum sample_Li(const Interaction &ref, const Vector2f &u,
                               Vector3f &wi, Float &pdf, VisibilityTester &vis) const = 0;

    virtual Float pdf_Li(const Interaction &, const Vector3f &) const = 0;

    //Emission
    virtual Spectrum Le(const Ray &r) const;

    virtual Spectrum sample_Le(const Vector2f &u1, const Vector2f &u2, Ray &ray,
                               Vector3f &nLight, Float &pdfPos, Float &pdfDir) const = 0;

    virtual void pdf_Le(const Ray &, const Vector3f &, Float &pdfPos, Float &pdfDir) const = 0;

    virtual ClassType getClassType() const override { return ClassType::Light; }

    int m_flags;
    int m_nSamples;

protected:
    Transform m_lightToWorld, m_worldToLight;
};

class VisibilityTester final
{
public:
    VisibilityTester() {}
    VisibilityTester(const Interaction &p0, const Interaction &p1)
        : m_p0(p0), m_p1(p1) {}

    const Interaction &P0() const { return m_p0; }
    const Interaction &P1() const { return m_p1; }

    bool unoccluded(const Scene &scene) const;

    Spectrum tr(const Scene &scene, Sampler &sampler) const;

private:
    Interaction m_p0, m_p1;
};

class AreaLight : public Light
{
public:
    typedef std::shared_ptr<AreaLight> ptr;

    AreaLight(const PropertyList &props);
    AreaLight(const Transform &lightToWorld, int nSamples);
    virtual Spectrum L(const Interaction &intr, const Vector3f &w) const = 0;
};

PLATINUM_END
#endif