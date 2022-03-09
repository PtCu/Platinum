

#include <core/primitive.h>

namespace platinum
{

    GeometricPrimitive::GeometricPrimitive(Ptr<Shape> shape, const Material *material,
                                           Ptr<AreaLight> area_light)
        : _shape(shape), _material(material), _area_light(area_light)
    {
        if (_area_light != nullptr)
        {
            _area_light->SetParent(this);
        }
    }

    bool GeometricPrimitive::Hit(const Ray &ray, SurfaceInteraction &inter) const
    {
        float t;
        if (!_shape->Hit(ray, t, inter))
            return false;
        ray._t_max = t;
        inter._hitable = this;
        return true;
    }

    void GeometricPrimitive::ComputeScatteringFunctions(SurfaceInteraction &inter, MemoryArena &arena) const
    {
        if (_material)
        {
            _material->ComputeScatteringFunctions(inter, arena);
        }
    }
}
