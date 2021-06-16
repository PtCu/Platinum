#ifndef MATERIALS_MIRROR_H_
#define MATERIALS_MIRROR_H_

#include "core/material.h"

namespace platinum
{
	class MirrorMaterial final : public Material
	{
	public:
		typedef std::shared_ptr<MirrorMaterial> ptr;

		MirrorMaterial(const PropertyTreeNode &node);
		MirrorMaterial(const Spectrum &r) : m_Kr(r) {}

		virtual void computeScatteringFunctions(SurfaceInteraction &si, MemoryArena &arena,
												TransportMode mode, bool allowMultipleLobes) const override;

		virtual std::string toString() const override { return "MirrorMaterial[]"; }

	private:
		Spectrum m_Kr;
	};
}

#endif