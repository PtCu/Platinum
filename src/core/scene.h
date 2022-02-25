

#ifndef CORE_WORLD_H_
#define CORE_WORLD_H_

#include <core/utilities.h>
#include <core/primitive.h>
#include <functional>
#include <shape/triangle.h>

namespace platinum
{
    class Scene
    {
    public:
        Scene() = default;

        Scene(UPtr<Aggregate> aggre, const std::vector<Ptr<Light>> &lights);

        void Initialize();

        bool Hit(const Ray &ray, SurfaceInteraction &inter) const;

        bool Hit(const Ray &ray) const;

        const Bounds3f &WorldBound() const { return _worldbound; }

        std::vector<Ptr<Light>> _lights;
        std::vector<Ptr<Light>> _infinite_lights;

        std::vector<UPtr<Transform>> _transforms;
        std::vector<UPtr<TriangleMesh>> _meshes;
        UPtr<Aggregate> _aggres;

    private:
        Bounds3f _worldbound;
    };

} // namespace platinum

#endif
