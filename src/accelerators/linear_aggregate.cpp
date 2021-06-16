#include "linear_aggregate.h"

namespace platinum
{
	LinearAggregate::LinearAggregate(const std::vector<Hitable::ptr> &hitables)
		: m_hitableList(hitables)
	{
		for (const auto &hitable : m_hitableList)
		{
			m_worldBounds = unionBounds(m_worldBounds, hitable->worldBound());
		}
	}

	Bounds3f LinearAggregate::worldBound() const { return m_worldBounds; }

	bool LinearAggregate::hit(const Ray &ray) const
	{
		for (int i = 0; i < m_hitableList.size(); i++)
		{
			if (m_hitableList[i]->hit(ray))
			{
				return true;
			}
		}
		return false;
	}

	bool LinearAggregate::hit(const Ray &ray, SurfaceInteraction &ret) const
	{
		SurfaceInteraction temp_rec;
		bool hit_anything = false;
		for (int i = 0; i < m_hitableList.size(); i++)
		{
			if (m_hitableList[i]->hit(ray, temp_rec))
			{
				hit_anything = true;
				ret = temp_rec;
			}
		}
		return hit_anything;
	}
}