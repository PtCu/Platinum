#ifndef CORE_SPECTRUM_H_
#define CORE_SPECTRUM_H_

#include "core/platinum.h"
#include "core/primitive.h"

namespace platinum
{
    template <int nSpectrumSamples>
    class CoefficientSpectrum
    {
    };

    class RGBSpectrum : public CoefficientSpectrum<3>
    {
    };
}


#endif