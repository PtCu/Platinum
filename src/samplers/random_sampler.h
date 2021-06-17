#ifndef ARRANDOMSAMPLER_H
#define ARRANDOMSAMPLER_H

#include "ArSampler.h"

namespace platinum
{
	class ARandomSampler final : public Sampler
	{
	public:
		typedef std::shared_ptr<ARandomSampler> ptr;

		ARandomSampler(const PropertyTreeNode &node);
		ARandomSampler(int ns, int seed = 0);

		virtual void startPixel(const AVector2i &) override;

		virtual Float get1D() override;
		virtual AVector2f get2D() override;

		virtual std::unique_ptr<Sampler> clone(int seed) override;

		virtual std::string toString() const override { return "RandomSampler[]"; }

	private:
		ARng m_rng; //Random number generator
	};
}

#endif