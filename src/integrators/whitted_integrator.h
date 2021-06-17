#ifndef INTEGRATORS_WHITTED_INTEGRATOR_H_
#define INTEGRATORS_WHITTED_INTEGRATOR_H_

#include "core/integrator.h"

namespace platinum
{
	class WhittedIntegrator : public SamplerIntegrator
	{
	public:
		typedef std::shared_ptr<WhittedIntegrator> ptr;

		WhittedIntegrator(const PropertyTreeNode &node);
		WhittedIntegrator(int maxDepth, Camera::ptr camera, Sampler::ptr sampler)
			: SamplerIntegrator(camera, sampler), m_maxDepth(maxDepth) {}

		virtual Spectrum Li(const Ray &ray, const Scene &scene,
							Sampler &sampler, MemoryArena &arena, int depth) const override;

		virtual std::string toString() const override { return "WhittedIntegrator[]"; }

	private:
		const int m_maxDepth;
	};
}

#endif