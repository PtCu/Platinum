#include <core/parser.h>

#include <fstream>
#include <exception>
#include <material/matte.h>
#include <light/diffuse_light.h>

namespace platinum
{

    void Parser::Parse(const std::string& path, Ptr<Scene> scene, Ptr<Integrator> integrator)
    {
        PropertyNode root;

        try
        {
            boost::property_tree::read_json(path, root);
        }
        catch (boost::property_tree::json_parser::json_parser_error&)
        {
            LOG(ERROR) << "Could not open the json file " << path << ", or file format error!";
        }
        LOG(INFO) << "Parse the scene file from " << path;

        _assets_path = root.get_value<std::string>("AssetsPath", "");

        auto integrator_node = root.get_child_optional("Integrator");
        if (!integrator_node)
        {
            LOG(ERROR) << "No integrator contained!";
            return;
        }
        auto _integrator = Ptr<Integrator>(static_cast<Integrator*>(ObjectFactory::CreateInstance(integrator_node->get<std::string>("Type", "Path"),
            integrator_node.get())));

        std::unordered_map<std::string, Ptr<Material>> materials;

        ParseMaterial(root, materials);

        std::vector<Ptr<Light>> lights;
        ParseLight(root, lights);

        std::vector<Ptr<Primitive>> ps;
        // ParseObject(root, ps, lights);



    }
    void Parser::ParseLight(const PropertyNode& root, std::vector<Ptr<Light>>& lights) const
    {
        if (!root.get_child_optional("Light"))
            return;
        for (const auto& p : root.get_child("Light"))
        {
            auto _light = Ptr<Light>(static_cast<Light*>(ObjectFactory::CreateInstance(p.second.get<std::string>("Type"), p.second)));
            lights.push_back(_light);
        }
    }

    void Parser::ParseMaterial(const PropertyNode& root, std::unordered_map<std::string, Ptr<Material>>& materials) const
    {
        if (!root.get_child_optional("Material"))
        {
            LOG(WARNING) << "No material contained! Use default material instead!";
            //默认材质
            materials["default"] = std::make_shared<Matte>();
            return;
        }

        for (const auto& p : root.get_child("Material"))
        {
            auto _name = p.second.get<std::string>("Name");
            auto _material = Ptr<Material>(static_cast<Material*>(ObjectFactory::CreateInstance(p.second.get<std::string>("Type"), p.second)));
            materials[_name] = _material;
        }
    }

    void Parser::ParseTransform(const PropertyNode& transform_node, Transform* obj2world) const
    {
        std::vector<Transform> transforms;
        for (const auto& trans : transform_node)
        {
            auto type = trans.second.get_value<std::string>("type");
            if ("translate" == type)
            {
                auto _translate_node = trans.second.get_child("value");
                auto iter = _translate_node.begin();
                Vector3f translate{};
                for (size_t i = 0; i < 3; ++i)
                {
                    translate[i++] = (iter++)->second.get_value<float>();
                }
                transforms.push_back(Translate(translate));
            }
            else if ("scale" == type)
            {
                auto _scale_node = trans.second.get_child("value");
                auto iter = _scale_node.begin();
                Vector3f scale{};
                for (size_t i = 0; i < 3; ++i)
                {
                    scale[i++] = (iter++)->second.get_value<float>();
                }

                transforms.push_back(Scale(scale));
            }
            else if ("rotate" == type)
            {
                auto _rotate_node = trans.second.get_child("value");
                //第一个数为旋转度数
                auto iter = _rotate_node.begin();
                float theta = iter->second.get_value<float>();
                //后三个数为旋转轴
                Vector3f axis{};
                for (size_t i = 0; i < 3; ++i)
                {
                    axis[i++] = (++iter)->second.get_value<float>();
                }

                transforms.push_back(Rotate(theta, axis));
            }
        }

        for (auto tr = transforms.begin(); tr != transforms.end(); ++tr)
        {
            (*obj2world) = (*tr) * (*obj2world);
        }
    }

    void Parser::ParseTriMesh(const PropertyNode& _shape_node, const Transform* obj2world,
        const Transform* world2obj, const std::optional<Spectrum> sp,
        const std::unordered_map<std::string, Ptr<Material>>& materials,
        std::vector<UPtr<TriangleMesh>>& trimesh)const {
        auto mesh_path = _shape_node.get<std::string>("Filename");
        auto mesh = std::make_unique<TriangleMesh>(obj2world, mesh_path);

        trimesh.emplace_back(mesh);
        const auto& meshIndices = mesh->GetIndices();
        for (size_t i = 0; i < meshIndices.size(); i += 3)
        {
            std::array<int, 3> indices;
            indices[0] = meshIndices[i + 0];
            indices[1] = meshIndices[i + 1];
            indices[2] = meshIndices[i + 2];
            auto triangle = std::make_shared<Triangle>(obj2world, world2obj, indices, mesh);

        }

    }
    void Parser::ParseObject(const PropertyNode& root, const std::unordered_map<std::string, Ptr<Material>>& materials, std::vector<Ptr<Primitive>>& primitives,
            std::vector<Ptr<Light>>& lights, std::vector<UPtr<Transform>>& transform, std::vector<UPtr<TriangleMesh>>& trimesh) const
    {

        for (const auto& p : root.get_child("Object"))
        {
            //对于列表中的元素value，p为(,value)。用p.second来访问
            auto obj2world = std::make_unique<Transform>();
            auto world2obj = std::make_unique<Transform>();

            auto _transform_node = p.second.get_child_optional("Transform");
            if (_transform_node)
            {
                ParseTransform(_transform_node.get(), obj2world.get());
            }
            (*world2obj) = Inverse(*obj2world);
            auto obj2world_ptr = obj2world.get();

            auto _light_node = p.second.get_child_optional("Emission");
            std::optional<Spectrum> spectrum_opt;
            if (_light_node) {
                auto spectrum_node = _light_node.get().get_child("value");
                auto iter = spectrum_node.begin();
                std::array<float, 3> spectrum;
                for (size_t i = 0; i < 3; ++i)
                {
                    spectrum[i++] = (iter++)->second.get_value<float>();
                }
                spectrum_opt.emplace(spectrum);

            }
            else {
                spectrum_opt = std::nullopt;
            }

            auto _shape_node = p.second.get_child("Shape");
            if ("Mesh" == _shape_node.get<std::string>("Type"))
            {


            }


            transform.emplace_back(obj2world);
            transform.emplace_back(world2obj);
        }
    }

}