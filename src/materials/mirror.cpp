#include "mirror.h"

#include "memory.h"
#include "bxdf.h"

namespace platinum
{
	PLT_REGISTER_CLASS(MirrorMaterial, "Mirror")

	MirrorMaterial::MirrorMaterial(const PropertyTreeNode &node)
	{
		const auto &props = node.getPropertyList();
		Vector3f _kr = props.getVector3f("R");
		Float _tmp[] = {_kr.x, _kr.y, _kr.z};
		m_Kr = Spectrum::fromRGB(_tmp);
		activate();
	}

	void MirrorMaterial::computeScatteringFunctions(SurfaceInteraction &si, MemoryArena &arena,
													 TransportMode mode, bool allowMultipleLobes) const
	{
		si.bsdf = ARENA_ALLOC(arena, BSDF)(si);
		Spectrum R = m_Kr;
		if (!R.isBlack())
		{
			si.bsdf->add(ARENA_ALLOC(arena, SpecularReflection)(
				R, ARENA_ALLOC(arena, FresnelNoOp)()));
		}
	}
}