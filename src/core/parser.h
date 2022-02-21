#ifndef CORE_PARSER_H_
#define CORE_PARSER_H_

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
        void Parse(const std::string &path, Ptr<Scene> &scene, Ptr<Integrator> &integrator);

        Parser(const Parser &) = delete;

        Parser &operator=(const Parser &) = delete;

    private:
        Parser() = default;

        void ParseLight(const PropertyTree &root);

        void ParseMaterial(const PropertyTree &root);

        void ParseObject(const PropertyTree &root);

        void ParseAggregate(const PropertyTree &root);

        void ParseTransform(const PropertyTree &root, Transform *transform);

        void ParseTriMesh(const PropertyTree &root, Transform *obj2world, Transform *world2obj);

        void ParseSimpleShape(const PropertyTree &root, Transform *obj2world, Transform *world2obj);

    private:
        std::string _assets_path;
        std::unordered_map<std::string, Ptr<Material>> _materials;
        std::vector<Ptr<Primitive>> _primitives;

        //对scene修改
        Ptr<Scene> _scene;
    };
}
#endif