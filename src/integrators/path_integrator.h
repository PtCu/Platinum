#ifndef INTEGRATORS_PATH_INTEGRATOR_H_
#define INTEGRATORS_PATH_INTEGRATOR_H_

#include "core/platinum.h"
#include "core/math_utils.h"
#include "core/integrator.h"
#include "core/pdf.h"

namespace platinum
{
	class PathIntegrator : public SamplerIntegrator
	{
	public:
		PathIntegrator(const PropertyTreeNode &props);

		PathIntegrator(int maxDepth, Camera::ptr camera, Sampler::ptr sampler,
					   Float rrThreshold = 1, const std::string &lightSampleStrategy = "spatial");

		virtual void preprocess(const Scene &scene) override;

		virtual Spectrum Li(const Ray &ray, const Scene &scene, Sampler &sampler,
							MemoryArena &arena, int depth) const override;

		virtual std::string toString() const override { return "PathIntegrator[]"; }

	private:
		// PathIntegrator Private Data
		int m_maxDepth;
		Float m_rrThreshold;
		std::string m_lightSampleStrategy;
		std::unique_ptr<ALightDistribution> m_lightDistribution;
	};

}

#endif