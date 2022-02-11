#include <core/parser.h>

#include <fstream>
#include <exception>
#include <material/matte.h>
#include <light/diffuse_light.h>
#include <accelerator/linear.h>
namespace platinum
{

    void Parser::Parse(const std::string &path, Ptr<Scene> scene, Ptr<Integrator> integrator)
    {
        scene = std::make_shared<Scene>();

        _scene = scene;

        PropertyNode root;
        try
        {
            boost::property_tree::read_json(path, root);

            LOG(INFO) << "Parse the scene file from " << path;

            _assets_path = root.get<std::string>("AssetsPath", "");

            auto integrator_node = root.get_child_optional("Integrator");
            if (!integrator_node)
            {
                LOG(ERROR) << "No integrator contained!";
                return;
            }
            integrator = Ptr<Integrator>(static_cast<Integrator *>(ObjectFactory::CreateInstance(integrator_node->get<std::string>("Type", "Path"),
                                                                                                 integrator_node.get())));

            ParseMaterial(root);

            ParseLight(root);

            ParseObject(root);

            for (const auto &p : _primitives)
            {
                if (p->GetAreaLight())
                {
                    _scene->_lights.emplace_back(std::static_pointer_cast<Light>(dynamic_cast<GeometricPrimitive *>(p.get())->GetAreaLightPtr()));
                }
            }

            ParseAggregate(root);

            
        }
        catch (boost::property_tree::json_parser::json_parser_error &)
        {
            LOG(ERROR) << "Could not open the json file " << path << ", or file format error!";
        }
    }

    void Parser::ParseAggregate(const PropertyNode &root)
    {
        auto aggre_node = root.get_child_optional("Aggregate");
        if (!aggre_node)
        {
            _scene->_aggres = UPtr<Aggregate>(new LinearAggregate);
        }
        _scene->_aggres = UPtr<Aggregate>(static_cast<Aggregate *>(ObjectFactory::CreateInstance(aggre_node->get<std::string>("Type"), aggre_node.get())));
        _scene->_aggres->SetPrimitives(_primitives);
    }

    void Parser::ParseLight(const PropertyNode &root)
    {
        if (!root.get_child_optional("Light"))
            return;
        for (const auto &p : root.get_child("Light"))
        {
            auto _light = Ptr<Light>(static_cast<Light *>(ObjectFactory::CreateInstance(p.second.get<std::string>("Type"), p.second)));
            _scene->_lights.emplace_back(_light);
        }
    }

    void Parser::ParseMaterial(const PropertyNode &root)
    {
        if (!root.get_child_optional("Material"))
        {
            LOG(WARNING) << "No material contained! Use default material instead!";
            //默认材质
            _materials["default"] = std::make_shared<Matte>();
            return;
        }

        for (const auto &p : root.get_child("Material"))
        {
            auto _name = p.second.get<std::string>("Name");
            auto _material = Ptr<Material>(static_cast<Material *>(ObjectFactory::CreateInstance(p.second.get<std::string>("Type"), p.second)));
            _materials[_name] = _material;
        }
    }

    void Parser::ParseTriMesh(const PropertyNode &root, Transform *obj2world,
                              Transform *world2obj)
    {
        auto mesh_path = root.get<std::string>("Shape.Filename");
        auto mesh = std::make_unique<TriangleMesh>(obj2world, mesh_path);
        const auto mat_string = root.get_optional<std::string>("Material");
        Ptr<Material> material = nullptr;
        if (!mat_string || _materials.find(mat_string.get()) == _materials.end())
        {
            material = _materials["default"];
        }
        material = _materials[mat_string.get()];

        auto is_emit = root.get_child_optional("Emission");

        auto &meshIndices = mesh->GetIndices();

        for (size_t i = 0; i < meshIndices.size(); i += 3)
        {
            std::array<int, 3> indices;
            indices[0] = meshIndices[i + 0];
            indices[1] = meshIndices[i + 1];
            indices[2] = meshIndices[i + 2];
            auto triangle = std::make_shared<Triangle>(obj2world, world2obj, indices, mesh.get());
            Ptr<AreaLight> area_light = nullptr;
            if (is_emit)
            {
                area_light = Ptr<AreaLight>(static_cast<AreaLight *>(ObjectFactory::CreateInstance("DiffuseAreaLight", is_emit.get())));
            }
            _primitives.emplace_back(std::make_shared<GeometricPrimitive>(triangle, material.get(), area_light));
        }

        _scene->_meshes.emplace_back(std::move(mesh));
    }

    void Parser::ParseSimpleShape(const PropertyNode &root, Transform *obj2world, Transform *world2obj)
    {
        const auto mat_string = root.get_optional<std::string>("Material");
        Ptr<Material> material = nullptr;
        if (!mat_string || _materials.find(mat_string.get()) == _materials.end())
        {
            material = _materials["default"];
        }
        material = _materials[mat_string.get()];

        auto shape = Ptr<Shape>(static_cast<Shape *>(ObjectFactory::CreateInstance(root.get<std::string>("Shape.Type"), root.get_child("Shape"))));

        shape->SetTransform(obj2world, world2obj);

        auto is_emit = root.get_child_optional("Emission");

        Ptr<AreaLight> area_light = nullptr;
        if (is_emit)
        {
            area_light = Ptr<AreaLight>(static_cast<AreaLight *>(ObjectFactory::CreateInstance("DiffuseAreaLight", is_emit.get())));
        }

        _primitives.emplace_back(std::make_shared<GeometricPrimitive>(shape, material.get(), area_light));
    }

    void Parser::ParseTransform(const PropertyNode &transform_node, Transform *obj2world)
    {
        std::vector<Transform> transforms;
        for (const auto &trans : transform_node)
        {
            auto type = trans.second.get<std::string>("type");
            if ("translate" == type)
            {
                auto _translate_node = trans.second.get_child("value");
                auto iter = _translate_node.begin();
                Vector3f translate;
                for (size_t i = 0; i < 3; ++i, ++iter)
                {
                    translate[i] = iter->second.get_value<float>(0.f);
                }
                transforms.emplace_back(Translate(translate));
            }
            else if ("scale" == type)
            {
                auto _scale_node = trans.second.get_child("value");
                auto iter = _scale_node.begin();
                Vector3f scale;
                for (size_t i = 0; i < 3; ++i, ++iter)
                {
                    scale[i] = iter->second.get_value<float>(0.f);
                }

                transforms.emplace_back(Scale(scale));
            }
            else if ("rotate" == type)
            {
                auto _rotate_node = trans.second.get_child("value");
                //第一个数为旋转度数
                auto iter = _rotate_node.begin();
                float theta = iter->second.get_value<float>(0.f);
                iter++;
                //后三个数为旋转轴
                Vector3f axis;
                for (size_t i = 0; i < 3; ++i, ++iter)
                {
                    axis[i] = iter->second.get_value<float>(0.f);
                }

                transforms.emplace_back(Rotate(theta, axis));
            }
        }

        for (auto tr = transforms.begin(); tr != transforms.end(); ++tr)
        {
            (*obj2world) = (*tr) * (*obj2world);
        }
    }

    void Parser::ParseObject(const PropertyNode &root)
    {

        for (const auto &p : root.get_child("Object"))
        {

            //解析transform
            auto obj2world = std::make_unique<Transform>();
            auto world2obj = std::make_unique<Transform>();

            //对于列表中的元素value，p为(,value)。用p.second来访问
            auto _transform_node = p.second.get_child_optional("Transform");
            if (_transform_node)
            {
                ParseTransform(_transform_node.get(), obj2world.get());
            }
            (*world2obj) = Inverse(*obj2world);

            //解析Shape
            if ("Mesh" == p.second.get<std::string>("Shape.Type"))
            {
                ParseTriMesh(p.second, obj2world.get(), world2obj.get());
            }
            else
            {
                ParseSimpleShape(p.second, obj2world.get(), world2obj.get());
            }

            _scene->_transforms.emplace_back(std::move(obj2world));
            _scene->_transforms.emplace_back(std::move(world2obj));
        }
    }
}