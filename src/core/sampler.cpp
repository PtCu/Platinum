#include "sampler.h"
#include "camera.h"

PLATINUM_BEGIN

//-------------------------------------------Sampler-------------------------------------

Sampler::~Sampler() {}

Sampler::Sampler(int64_t samplesPerPixel) : samplesPerPixel(samplesPerPixel) {}

Sampler::Sampler(const PropertyList &props) : samplesPerPixel(props.getInteger("SPP", 1)) {}

CameraSample Sampler::getCameraSample(const Vector2i &pRaster)
{
    CameraSample cs;
    cs.pFilm = (Vector2f)pRaster + get2D();
    return cs;
}

void Sampler::startPixel(const Vector2i &p)
{
    m_currentPixel = p;
    m_currentPixelSampleIndex = 0;
    // Reset array offsets for next pixel sample
    m_array1DOffset = m_array2DOffset = 0;
}

bool Sampler::startNextSample()
{
    // Reset array offsets for next pixel sample
    m_array1DOffset = m_array2DOffset = 0;
    return ++m_currentPixelSampleIndex < samplesPerPixel;
}

bool Sampler::setSampleNumber(int64_t sampleNum)
{
    // Reset array offsets for next pixel sample
    m_array1DOffset = m_array2DOffset = 0;
    m_currentPixelSampleIndex = sampleNum;
    return m_currentPixelSampleIndex < samplesPerPixel;
}

void Sampler::request1DArray(int n)
{
    CHECK_EQ(roundCount(n), n);
    m_samples1DArraySizes.push_back(n);
    m_sampleArray1D.push_back(std::vector<Float>(n * samplesPerPixel));
}

void Sampler::request2DArray(int n)
{
    CHECK_EQ(roundCount(n), n);
    m_samples2DArraySizes.push_back(n);
    m_sampleArray2D.push_back(std::vector<Vector2f>(n * samplesPerPixel));
}

const Float *Sampler::get1DArray(int n)
{
    if (m_array1DOffset == m_sampleArray1D.size())
        return nullptr;
    CHECK_EQ(m_samples1DArraySizes[m_array1DOffset], n);
    CHECK_LT(m_currentPixelSampleIndex, samplesPerPixel);
    return &m_sampleArray1D[m_array1DOffset++][m_currentPixelSampleIndex * n];
}

const Vector2f *Sampler::get2DArray(int n)
{
    if (m_array2DOffset == m_sampleArray2D.size())
        return nullptr;
    CHECK_EQ(m_samples2DArraySizes[m_array2DOffset], n);
    CHECK_LT(m_currentPixelSampleIndex, samplesPerPixel);
    return &m_sampleArray2D[m_array2DOffset++][m_currentPixelSampleIndex * n];
}

//-------------------------------------------SamplingAlgorithm-------------------------------------

Vector3f uniformSampleHemisphere(const Vector2f &u)
{
    Float z = u[0];
    Float r = glm::sqrt(glm::max((Float)0, (Float)1. - z * z));
    Float phi = 2 * aPi * u[1];
    return Vector3f(r * glm::cos(phi), r * glm::sin(phi), z);
}

Float uniformHemispherePdf() { return aInv2Pi; }

Vector3f uniformSampleSphere(const Vector2f &u)
{
    Float z = 1 - 2 * u[0];
    Float r = glm::sqrt(glm::max((Float)0, (Float)1 - z * z));
    Float phi = 2 * aPi * u[1];
    return Vector3f(r * glm::cos(phi), r * glm::sin(phi), z);
}

Float uniformSpherePdf() { return aInv4Pi; }

Vector3f uniformSampleCone(const Vector2f &u, Float cosThetaMax)
{
    Float cosTheta = ((Float)1 - u[0]) + u[0] * cosThetaMax;
    Float sinTheta = glm::sqrt((Float)1 - cosTheta * cosTheta);
    Float phi = u[1] * 2 * aPi;
    return Vector3f(glm::cos(phi) * sinTheta, glm::sin(phi) * sinTheta, cosTheta);
}

Vector3f uniformSampleCone(const Vector2f &u, Float cosThetaMax, const Vector3f &x,
                           const Vector3f &y, const Vector3f &z)
{
    Float cosTheta = lerp(u[0], cosThetaMax, 1.f);
    Float sinTheta = glm::sqrt((Float)1. - cosTheta * cosTheta);
    Float phi = u[1] * 2 * aPi;
    return glm::cos(phi) * sinTheta * x + glm::sin(phi) * sinTheta * y + cosTheta * z;
}

Float uniformConePdf(Float cosThetaMax) { return 1 / (2 * aPi * (1 - cosThetaMax)); }

Vector2f concentricSampleDisk(const Vector2f &u)
{
    // Map uniform random numbers to $[-1,1]^2$
    Vector2f uOffset = 2.f * u - Vector2f(1, 1);

    // Handle degeneracy at the origin
    if (uOffset.x == 0 && uOffset.y == 0)
        return Vector2f(0, 0);

    // Apply concentric mapping to point
    Float theta, r;
    if (glm::abs(uOffset.x) > glm::abs(uOffset.y))
    {
        r = uOffset.x;
        theta = aPiOver4 * (uOffset.y / uOffset.x);
    }
    else
    {
        r = uOffset.y;
        theta = aPiOver2 - aPiOver4 * (uOffset.x / uOffset.y);
    }
    return r * Vector2f(glm::cos(theta), glm::sin(theta));
}

Vector2f uniformSampleTriangle(const Vector2f &u)
{
    Float su0 = glm::sqrt(u[0]);
    return Vector2f(1 - su0, u[1] * su0);
}

PLATINUM_END