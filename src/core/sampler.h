#ifndef CORE_SAMPLER_H_
#define CORE_SAMPLER_H_

#include "platinum.h"
#include "math_utils.h"
#include "core/random.h"
#include "rtti.h"

#include <vector>
namespace platinum
{

    class Sampler : public Object
    {
    public:
        typedef std::shared_ptr<Sampler> ptr;

        virtual ~Sampler();
        Sampler(const PropertyList &props);
        Sampler(int64_t samplesPerPixel);

        virtual void startPixel(const Vector2i &p);
        virtual Float get1D() = 0;
        virtual Vector2f get2D() = 0;
        CameraSample getCameraSample(const Vector2i &pRaster);

        void request1DArray(int n);
        void request2DArray(int n);

        virtual int roundCount(int n) const { return n; }

        const Float *get1DArray(int n);
        const Vector2f *get2DArray(int n);
        virtual bool startNextSample();

        virtual std::unique_ptr<Sampler> clone(int seed) = 0;
        virtual bool setSampleNumber(int64_t sampleNum);

        int64_t currentSampleNumber() const { return m_currentPixelSampleIndex; }

        int64_t getSamplingNumber() const { return samplesPerPixel; }

        virtual ClassType getClassType() const override { return ClassType::SamplerType; }

        const int64_t samplesPerPixel; //Number of sampling per pixel

    protected:
        Vector2i m_currentPixel;
        int64_t m_currentPixelSampleIndex;
        std::vector<int> m_samples1DArraySizes, m_samples2DArraySizes;
        std::vector<std::vector<Float>> m_sampleArray1D;
        std::vector<std::vector<Vector2f>> m_sampleArray2D;

    private:
        size_t m_array1DOffset, m_array2DOffset;
    };

    Vector3f uniformSampleHemisphere(const Vector2f &u);
    Float uniformHemispherePdf();
    Vector3f uniformSampleSphere(const Vector2f &u);
    Float uniformSpherePdf();

    Vector3f uniformSampleCone(const Vector2f &u, Float thetamax);
    Vector3f uniformSampleCone(const Vector2f &u, Float thetamax, const Vector3f &x,
                               const Vector3f &y, const Vector3f &z);
    Float uniformConePdf(Float thetamax);

    Vector2f concentricSampleDisk(const Vector2f &u);

    Vector2f uniformSampleTriangle(const Vector2f &u);

    inline Vector3f cosineSampleHemisphere(const Vector2f &u)
    {
        Vector2f d = concentricSampleDisk(u);
        Float z = std::sqrt(glm::max((Float)0, 1 - d.x * d.x - d.y * d.y));
        return Vector3f(d.x, d.y, z);
    }

    inline Float cosineHemispherePdf(Float cosTheta) { return cosTheta * InvPi; }

    inline Float balanceHeuristic(int nf, Float fPdf, int ng, Float gPdf)
    {
        return (nf * fPdf) / (nf * fPdf + ng * gPdf);
    }

    inline Float powerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
    {
        Float f = nf * fPdf, g = ng * gPdf;
        return (f * f) / (f * f + g * g);
    }
}

#endif