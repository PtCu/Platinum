#include "ArRandomSampler.h"

namespace platinum
{
	PLT_REGISTER_CLASS(ARandomSampler, "Random")

	ARandomSampler::ARandomSampler(const PropertyTreeNode &node) : Sampler(node.getPropertyList()), m_rng(0) { activate(); }

	ARandomSampler::ARandomSampler(int ns, int seed) : Sampler(ns), m_rng(seed) {}

	Float ARandomSampler::get1D()
	{
		CHECK_LT(m_currentPixelSampleIndex, samplesPerPixel);
		return m_rng.uniformFloat();
	}

	AVector2f ARandomSampler::get2D()
	{
		CHECK_LT(m_currentPixelSampleIndex, samplesPerPixel);
		return {m_rng.uniformFloat(), m_rng.uniformFloat()};
	}

	std::unique_ptr<Sampler> ARandomSampler::clone(int seed)
	{
		ARandomSampler *rs = new ARandomSampler(*this);
		rs->m_rng.setSequence(seed);
		return std::unique_ptr<Sampler>(rs);
	}

	void ARandomSampler::startPixel(const AVector2i &p)
	{
		for (size_t i = 0; i < m_sampleArray1D.size(); ++i)
			for (size_t j = 0; j < m_sampleArray1D[i].size(); ++j)
				m_sampleArray1D[i][j] = m_rng.uniformFloat();

		for (size_t i = 0; i < m_sampleArray2D.size(); ++i)
			for (size_t j = 0; j < m_sampleArray2D[i].size(); ++j)
				m_sampleArray2D[i][j] = {m_rng.uniformFloat(), m_rng.uniformFloat()};

		Sampler::startPixel(p);
	}

}