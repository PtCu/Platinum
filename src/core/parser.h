#ifndef CORE_PARSER_H_
#define CORE_PARSER_H_

#include <boost/property_tree/json_parser.hpp>
#include <optional>
#include <core/object.h>
#include <core/utilities.h>
#include <core/integrator.h>
#include <core/scene.h>
#include <core/camera.h>
#include <core/sampler.h>
#include <core/film.h>
#include <core/singleton.h>
#include <shape/triangle.h>
namespace platinum
{

    /**
     * @brief 解析类，实现从描述文件创建场景
     *
     */
    class Parser final : public Singleton<Parser>
    {
        friend class Singleton<Parser>;

    public:
        void Parse(const std::string& path, Ptr<Scene> scene, Ptr<Integrator> integrator);

        Parser(const Parser&) = delete;

        Parser& operator=(const Parser&) = delete;

    private:
        Parser() = default;

        void ParseLight(const PropertyNode& root);

        void ParseMaterial(const PropertyNode& root);

        void ParseObject(const PropertyNode& root) ;

        void ParseTransform(const PropertyNode& root, Transform* transform) ;

        void ParseTriMesh(const PropertyNode& root, Transform* obj2world, Transform* world2obj);

        void ParseSimpleShape(const PropertyNode &root, Transform *obj2world, Transform *world2obj);

    private:
        std::string _assets_path;
        std::vector<Ptr<Light>> &_lights;
        std::unordered_map<std::string, Ptr<Material>> &_materials;
        std::vector<Ptr<Primitive>> &_primitives;
        std::vector<UPtr<Transform>> &_transforms;
        std::vector<UPtr<TriangleMesh>> &_meshes;
    };
}
#endif