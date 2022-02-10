#include <core/parser.h>

#include <fstream>
#include <exception>
#include <material/matte.h>

namespace platinum
{

    void Parser::Parse(const std::string &path, Ptr<Scene> scene, Ptr<Integrator> integrator) const
    {
        PropertyNode root;

        try
        {
            boost::property_tree::read_json(path, root);
        }
        catch (boost::property_tree::json_parser::json_parser_error &)
        {
            LOG(ERROR) << "Could not open the json file " << path << ", or file format error!";
        }
        LOG(INFO) << "Parse the scene file from " << path;

        auto integrator_node = root.get_child_optional("Integrator");
        if (!integrator_node)
        {
            LOG(ERROR) << "No integrator contained!";
            return;
        }
        auto _integrator = Ptr<Integrator>(static_cast<Integrator *>(ObjectFactory::CreateInstance(integrator_node->get<std::string>("Type", "Path"),
                                                                                                   integrator_node.get())));

        std::unordered_map<std::string, Ptr<Material>> materials;

        ParseMaterial(root, materials);

        std::vector<Ptr<Light>> lights;
        ParseLight(root, lights);

        std::vector<Ptr<Primitive>> ps;
        ParseObject(root, ps, lights);
    }
    void Parser::ParseLight(const PropertyNode &root, std::vector<Ptr<Light>> &lights) const
    {
        if (!root.get_child_optional("Light"))
            return;
        for (const auto &p : root.get_child("Light"))
        {
            auto _light = Ptr<Light>(static_cast<Light *>(ObjectFactory::CreateInstance(p.second.get<std::string>("Type"), p.second)));
            lights.push_back(_light);
        }
    }

    void Parser::ParseMaterial(const PropertyNode &root, std::unordered_map<std::string, Ptr<Material>> &materials) const
    {
        if (!root.get_child_optional("Material"))
        {
            LOG(WARNING) << "No material contained! Use default material instead!";
            //默认材质
            materials["default"] = std::make_shared<Matte>();
            return;
        }

        for (const auto &p : root.get_child("Material"))
        {
            auto _name = p.second.get<std::string>("Name");
            auto _material = Ptr<Material>(static_cast<Material *>(ObjectFactory::CreateInstance(p.second.get<std::string>("Type"), p.second)));
            materials[_name] = _material;
        }
    }

    void Parser::ParseTransform(const PropertyNode &transform_node, std::vector<Transform> &transforms) const
    {
        for (const auto &trans : transform_node)
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
    }
    void Parser::ParseObject(const PropertyNode &root, std::vector<Ptr<Primitive>> &primitives, std::vector<Ptr<Light>> &lights) const
    {

        for (const auto &p : root.get_child("Object"))
        {
            //对于列表中的元素value，p为(,value)。用p.second来访问
            Transform obj2world{};
            std::vector<Transform> transforms;
            auto _transform_node = p.second.get_child_optional("Transform");
            if (_transform_node)
            {
                ParseTransform(_transform_node.get(), transforms);
            }

            for (auto tr = transforms.begin(); tr != transforms.end(); ++tr)
            {
                obj2world = (*tr) * obj2world;
            }

            auto _shape_node = p.second.get_child("Shape");
            if (_shape_node.get<std::string>("Type") == "Mesh")
            {
                
            }
        }
    }

}