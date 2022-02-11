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

        void ParseLight(const PropertyNode& root, std::vector<Ptr<Light>>& lights) const;

        void ParseMaterial(const PropertyNode& root, std::unordered_map<std::string, Ptr<Material>>& materials) const;

        void ParseObject(const PropertyNode& root, const std::unordered_map<std::string, Ptr<Material>>& materials, std::vector<Ptr<Primitive>>& primitives,
            std::vector<Ptr<Light>>& lights, std::vector<UPtr<Transform>>& transform, std::vector<UPtr<TriangleMesh>>& trimesh) const;

        void ParseTransform(const PropertyNode& trans, Transform* transform) const;

        void ParseTriMesh(const PropertyNode& root, const Transform* obj2world, const Transform* world2obj, const std::optional<Spectrum> sp, const std::unordered_map<std::string, Ptr<Material>>& materials, std::vector<UPtr<TriangleMesh>>& trimesh)const;

    private:
        std::string _assets_path;
    };
}
#endif