

#include <accelerator/linear.h>
#include <tbb/parallel_for.h>
#include <tbb/spin_mutex.h>
#include <core/timer.h>
namespace platinum
{

    REGISTER_CLASS(LinearAggregate, "Linear");

    LinearAggregate::LinearAggregate(const PropertyTree &node)
        : Aggregate(node)
    {
    }

    LinearAggregate::LinearAggregate(const std::vector<Ptr<Primitive>> &hitables)
        : Aggregate(hitables)
    {
        tbb::spin_mutex mtx;
        tbb::parallel_for(tbb::blocked_range<size_t>(0, _primitives.size()),
                          [&](tbb::blocked_range<size_t> r)
                          {
                              Bounds3f local_bound;
                              for (size_t i = r.begin(); i < r.end(); ++i)
                              {
                                  auto aabb = _primitives[i]->WorldBound();
                                  local_bound = UnionBounds(local_bound, aabb);
                                  // 储存每个aabb的中心以及索引
                               
                              }
                              std::lock_guard lck(mtx);
                              _world_bounds = UnionBounds(_world_bounds, local_bound);
                          });
       
    }

    void LinearAggregate::Initialize()
    {
        LOG(INFO) << "Construct Linear accelerator..";
        Timer timer("Linear aggregate initialize");
        for (const auto &hitable : _primitives)
        {
            _world_bounds = UnionBounds(_world_bounds, hitable->WorldBound());
        }
    }

    bool LinearAggregate::Hit(const Ray &ray) const
    {
        for (const auto &hitble : _primitives)
        {
            if (hitble->Hit(ray))
            {
                return true;
            }
        }
        return false;
    }

    bool LinearAggregate::Hit(const Ray &ray, SurfaceInteraction &inter) const
    {
        SurfaceInteraction tmp_inter;
        bool is_hit = false;
        for (const auto &hitble : _primitives)
        {
            if (hitble->Hit(ray, tmp_inter))
            {

                is_hit = true;
                inter = tmp_inter;
            }
        }
        return is_hit;
    }
}
    