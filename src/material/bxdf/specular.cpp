

#include <material/bxdf/specular.h>

namespace platinum
{
    Spectrum SpecularReflection::SampleF(const Vector3f &wo, Vector3f &wi, const Vector2f &sample, float &pdf, BxDFType &sampleType) const
    {
        wi = Vector3f(-wo.x, -wo.y, wo.z);
        pdf = 1.f;
        return _fresnel->Evaluate(wi.z) * _R / glm::abs(wi.z);
    }
}