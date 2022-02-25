

#include <core/texture.h>

namespace platinum
{
    /**
     * @brief  By providing this class, all parameters to all Materials can be represented as Textures,
            *  whether they are spatially varying or not. 
            *  For example, a red diffuse object will have a ConstantTexture that always returns 
            *  red as the diffuse color of the material. 
            *  This way, the shading system always evaluates a texture to get the surface properties at 
            *  a point, avoiding the need for separate textured and nontextured versions of materials.
            * 
     * @tparam T 
     */
    template <typename T>
    class ConstantTexture : public Texture<T>
    {
    public:
        // ConstantTexture Public Methods
        ConstantTexture(const PropertyTree &node){}
        ConstantTexture(const T &value) : value(value) {}
        T Evaluate(const SurfaceInteraction &) const { return value; }

        virtual std::string ToString() const { return "Sphere"; }

    private:
        T value;
    };
}
