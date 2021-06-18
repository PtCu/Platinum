#ifndef SAMPLERS_RANDOM_SAMPLER_H_
#define SAMPLERS_RANDOM_SAMPLER_H_

#include "core/sampler.h"

namespace platinum
{
	class RandomSampler final : public Sampler
	{
	public:
		typedef std::shared_ptr<RandomSampler> ptr;

		RandomSampler(const PropertyTreeNode &node);
		RandomSampler(int ns, int seed = 0);

		virtual void startPixel(const Vector2i &) override;

		virtual Float get1D() override;
		virtual Vector2f get2D() override;

		virtual std::unique_ptr<Sampler> clone(int seed) override;

		virtual std::string toString() const override { return "RandomSampler[]"; }

	private:
		Random m_rng; //Random number generator
	};
}

#endif