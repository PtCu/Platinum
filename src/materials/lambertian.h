#ifndef MATERIALS_LAMBERTIAN_H_
#define MATERIALS_LAMBERTIAN_H_

#include "core/material.h"

namespace platinum
{
	class LambertianMaterial final : public Material
	{
	public:
		typedef std::shared_ptr<LambertianMaterial> ptr;

		LambertianMaterial(const PropertyTreeNode &node);
		LambertianMaterial(const Spectrum &r) : m_Kr(r) {}

		virtual void computeScatteringFunctions(SurfaceInteraction &si, MemoryArena &arena,
												TransportMode mode, bool allowMultipleLobes) const override;

		virtual std::string toString() const override { return "LambertianMaterial[]"; }

	private:
		Spectrum m_Kr;
	};
}

#endif