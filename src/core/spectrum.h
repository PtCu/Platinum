

#ifndef CORE_SPECTRUM_H_
#define CORE_SPECTRUM_H_

#include <core/utilities.h>

namespace platinum
{
    inline void XYZToRGB(const float xyz[3], float rgb[3])
    {
        rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
        rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
        rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
    }

    inline void RGBToXYZ(const float rgb[3], float xyz[3])
    {
        xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
        xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
        xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];
    }

    enum class SpectrumType
    {
        Reflectance,
        Illuminant
    };

    template <int nSpectrumSamples>
    class CoefficientSpectrum
    {
    public:
        CoefficientSpectrum(float v = 0.f)
        {
            for (int i = 0; i < nSpectrumSamples; ++i)
                c[i] = v;
            DCHECK(!hasNaNs());
        }

        CoefficientSpectrum &operator+=(const CoefficientSpectrum &s2)
        {
            DCHECK(!s2.hasNaNs());
            for (int i = 0; i < nSpectrumSamples; ++i)
                c[i] += s2.c[i];
            return *this;
        }

        CoefficientSpectrum operator+(const CoefficientSpectrum &s2) const
        {
            DCHECK(!s2.hasNaNs());
            CoefficientSpectrum ret = *this;
            for (int i = 0; i < nSpectrumSamples; ++i)
                ret.c[i] += s2.c[i];
            return ret;
        }

        CoefficientSpectrum operator-(const CoefficientSpectrum &s2) const
        {
            DCHECK(!s2.hasNaNs());
            CoefficientSpectrum ret = *this;
            for (int i = 0; i < nSpectrumSamples; ++i)
                ret.c[i] -= s2.c[i];
            return ret;
        }

        CoefficientSpectrum operator/(const CoefficientSpectrum &s2) const
        {
            DCHECK(!s2.hasNaNs());
            CoefficientSpectrum ret = *this;
            for (int i = 0; i < nSpectrumSamples; ++i)
            {
                CHECK_NE(s2.c[i], 0);
                ret.c[i] /= s2.c[i];
            }
            return ret;
        }

        CoefficientSpectrum operator*(const CoefficientSpectrum &sp) const
        {
            DCHECK(!sp.hasNaNs());
            CoefficientSpectrum ret = *this;
            for (int i = 0; i < nSpectrumSamples; ++i)
                ret.c[i] *= sp.c[i];
            return ret;
        }

        CoefficientSpectrum &operator*=(const CoefficientSpectrum &sp)
        {
            DCHECK(!sp.hasNaNs());
            for (int i = 0; i < nSpectrumSamples; ++i)
                c[i] *= sp.c[i];
            return *this;
        }

        CoefficientSpectrum operator*(float a) const
        {
            CoefficientSpectrum ret = *this;
            for (int i = 0; i < nSpectrumSamples; ++i)
                ret.c[i] *= a;
            DCHECK(!ret.hasNaNs());
            return ret;
        }

        CoefficientSpectrum &operator*=(float a)
        {
            for (int i = 0; i < nSpectrumSamples; ++i)
                c[i] *= a;
            DCHECK(!hasNaNs());
            return *this;
        }

        friend inline CoefficientSpectrum operator*(float a, const CoefficientSpectrum &s)
        {
            DCHECK(!glm::isnan(a) && !s.hasNaNs());
            return s * a;
        }

        CoefficientSpectrum operator/(float a) const
        {
            CHECK_NE(a, 0);
            DCHECK(!glm::isnan(a));
            CoefficientSpectrum ret = *this;
            for (int i = 0; i < nSpectrumSamples; ++i)
                ret.c[i] /= a;
            DCHECK(!ret.hasNaNs());
            return ret;
        }

        CoefficientSpectrum &operator/=(float a)
        {
            CHECK_NE(a, 0);
            DCHECK(!glm::isnan(a));
            for (int i = 0; i < nSpectrumSamples; ++i)
                c[i] /= a;
            return *this;
        }

        bool operator==(const CoefficientSpectrum &sp) const
        {
            for (int i = 0; i < nSpectrumSamples; ++i)
            {
                if (c[i] != sp.c[i])
                    return false;
            }
            return true;
        }

        bool operator!=(const CoefficientSpectrum &sp) const
        {
            return !(*this == sp);
        }

        bool isBlack() const
        {
            for (int i = 0; i < nSpectrumSamples; ++i)
            {
                if (c[i] != 0.)
                    return false;
            }
            return true;
        }

        friend CoefficientSpectrum sqrt(const CoefficientSpectrum &s)
        {
            CoefficientSpectrum ret;
            for (int i = 0; i < nSpectrumSamples; ++i)
                ret.c[i] = glm::sqrt(s.c[i]);
            DCHECK(!ret.hasNaNs());
            return ret;
        }

        template <int n>
        friend inline CoefficientSpectrum<n> pow(const CoefficientSpectrum<n> &s, float e);

        CoefficientSpectrum operator-() const
        {
            CoefficientSpectrum ret;
            for (int i = 0; i < nSpectrumSamples; ++i)
                ret.c[i] = -c[i];
            return ret;
        }

        friend CoefficientSpectrum exp(const CoefficientSpectrum &s)
        {
            CoefficientSpectrum ret;
            for (int i = 0; i < nSpectrumSamples; ++i)
                ret.c[i] = glm::exp(s.c[i]);
            DCHECK(!ret.hasNaNs());
            return ret;
        }

        friend std::ostream &operator<<(std::ostream &os, const CoefficientSpectrum &s)
        {
            return os << s.toString();
        }

        std::string toString() const
        {
            std::string str = "[ ";
            for (int i = 0; i < nSpectrumSamples; ++i)
            {
                str += StringPrintf("%f", c[i]);
                if (i + 1 < nSpectrumSamples)
                    str += ", ";
            }
            str += " ]";
            return str;
        }

        CoefficientSpectrum clamp(float low = 0, float high = aInfinity) const
        {
            CoefficientSpectrum ret;
            for (int i = 0; i < nSpectrumSamples; ++i)
                ret.c[i] = Aurora::clamp(c[i], low, high);
            DCHECK(!ret.hasNaNs());
            return ret;
        }

        float maxComponentValue() const
        {
            float m = c[0];
            for (int i = 1; i < nSpectrumSamples; ++i)
                m = glm::max(m, c[i]);
            return m;
        }

        bool hasNaNs() const
        {
            for (int i = 0; i < nSpectrumSamples; ++i)
            {
                if (glm::isnan(c[i]))
                    return true;
            }
            return false;
        }

        float &operator[](int i)
        {
            DCHECK(i >= 0 && i < nSpectrumSamples);
            return c[i];
        }

        float operator[](int i) const
        {
            DCHECK(i >= 0 && i < nSpectrumSamples);
            return c[i];
        }

        static const int nSamples = nSpectrumSamples;

    protected:
        float c[nSpectrumSamples];
    };

    class RGBSpectrum : public CoefficientSpectrum<3>
    {
        using CoefficientSpectrum<3>::c;

    public:
        RGBSpectrum(float v = 0.f) : CoefficientSpectrum<3>(v) {}
        RGBSpectrum(const CoefficientSpectrum<3> &v) : CoefficientSpectrum<3>(v) {}
        RGBSpectrum(const RGBSpectrum &s) { *this = s; }
        RGBSpectrum(const std::array<float, 3> &rgb)
        {
            c[0] = rgb[0];
            c[1] = rgb[1];
            c[2] = rgb[2];
        }
        RGBSpectrum(float r, float g, float b)
        {
            c[0] = r;
            c[1] = g;
            c[2] = b;
        }
        static RGBSpectrum fromRGB(const float rgb[3])
        {
            RGBSpectrum s;
            s.c[0] = rgb[0];
            s.c[1] = rgb[1];
            s.c[2] = rgb[2];
            DCHECK(!s.hasNaNs());
            return s;
        }
        static RGBSpectrum fromRGB(const std::array<float, 3> &rgb)
        {
            RGBSpectrum s;
            s.c[0] = rgb[0];
            s.c[1] = rgb[1];
            s.c[2] = rgb[2];
            DCHECK(!s.hasNaNs());
            return s;
        }

        static RGBSpectrum fromRGB(const Vector3f &rgb)
        {
            RGBSpectrum s;
            s.c[0] = rgb[0];
            s.c[1] = rgb[1];
            s.c[2] = rgb[2];
            DCHECK(!s.hasNaNs());
            return s;
        }

        void toRGB(float *rgb) const
        {
            rgb[0] = c[0];
            rgb[1] = c[1];
            rgb[2] = c[2];
        }

        void toXYZ(float xyz[3]) const { RGBToXYZ(c, xyz); }
        const RGBSpectrum &toRGBSpectrum() const { return *this; }

        static RGBSpectrum fromXYZ(const float xyz[3], SpectrumType type = SpectrumType::Reflectance)
        {
            RGBSpectrum r;
            XYZToRGB(xyz, r.c);
            return r;
        }

        float y() const
        {
            const float YWeight[3] = {0.212671f, 0.715160f, 0.072169f};
            return YWeight[0] * c[0] + YWeight[1] * c[1] + YWeight[2] * c[2];
        }
    };

    // Spectrum Inline Functions
    template <int nSpectrumSamples>
    inline CoefficientSpectrum<nSpectrumSamples> pow(
        const CoefficientSpectrum<nSpectrumSamples> &s, float e)
    {
        CoefficientSpectrum<nSpectrumSamples> ret;
        for (int i = 0; i < nSpectrumSamples; ++i)
        {
            ret.c[i] = glm::pow(s.c[i], e);
        }
        DCHECK(!ret.hasNaNs());
        return ret;
    }
}

#endif