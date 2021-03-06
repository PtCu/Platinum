#include <core/parser.h>

#include <fstream>
#include <exception>
#include <material/matte.h>
#include <light/diffuse_light.h>
#include <accelerator/linear.h>
#include <material/mirror.h>
#include <integrator/whitted_integrator.h>
#include <tbb/parallel_for.h>
#include <core/timer.h>
namespace platinum
{

    void Parser::Parse(const std::string &path, Ptr<Scene> &scene, Ptr<Integrator> &integrator)
    {
        LOG(INFO) << "Start parsing scene...";
        scene = std::make_shared<Scene>();

        _scene = scene;

        PropertyTree root;

        try
        {
            root.ReadJson(path);

            LOG(INFO) << "Parse the scene file from " << path;

            _assets_path = root.Get<std::string>("AssetsPath");

            auto integrator_node = root.GetChildOptional("Integrator");
            if (!integrator_node)
            {
                LOG(ERROR) << "No integrator contained!";
                return;
            }

            integrator = Ptr<Integrator>(static_cast<Integrator *>(ObjectFactory::CreateInstance(integrator_node->Get<std::string>("Type", "Path"),
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

        scene->Initialize();
    }

    void Parser::ParseAggregate(const PropertyTree &root)
    {
        auto aggre_node = root.GetChildOptional("Aggregate");
        if (!aggre_node)
        {
            _scene->_aggres = UPtr<Aggregate>(new LinearAggregate);
        }
        _scene->_aggres = UPtr<Aggregate>(static_cast<Aggregate *>(ObjectFactory::CreateInstance(aggre_node->Get<std::string>("Type"), aggre_node.get())));
        _scene->_aggres->SetPrimitives(_primitives);
    }

    void Parser::ParseLight(const PropertyTree &root)
    {
        if (!root.GetChildOptional("Light"))
            return;
        for (const auto &p : root.GetChild("Light").GetNode())
        {
            auto _light = Ptr<Light>(static_cast<Light *>(ObjectFactory::CreateInstance(p.second.get<std::string>("Type"), p.second)));
            _scene->_lights.emplace_back(_light);
        }
    }

    void Parser::ParseMaterial(const PropertyTree &root)
    {
        if (!root.GetChildOptional("Material"))
        {
            LOG(WARNING) << "No material contained! Use default material instead!";
            //????????????
            _materials["default"] = std::make_shared<Matte>();
            return;
        }

        for (const auto &p : root.GetChild("Material").GetNode())
        {
            auto _name = p.second.get<std::string>("Name");
            auto _material = Ptr<Material>(static_cast<Material *>(ObjectFactory::CreateInstance(p.second.get<std::string>("Type"), p.second)));
            _materials[_name] = _material;
        }
    }

    void Parser::ParseTriMesh(const PropertyTree &root, Transform *obj2world,
                              Transform *world2obj)
    {
        Timer timer("Parse TriMesh");
        auto mesh_path = root.Get<std::string>("Shape.Filename");
        auto mesh = std::make_unique<TriangleMesh>(obj2world, _assets_path + mesh_path);
        const auto mat_string = root.GetOptional<std::string>("Material");
        Ptr<Material> material = nullptr;
        if (!mat_string || _materials.find(mat_string.get()) == _materials.end())
        {
            material = _materials["default"];
        }
        material = _materials[mat_string.get()];

        auto is_emit = root.GetChildOptional("Emission");

        auto &meshIndices = mesh->GetIndices();
        _primitives.reserve(_primitives.size() + meshIndices.size() / 3 + 1);

        if (meshIndices.size() > 20)
        {
            std::mutex mtx;
            tbb::parallel_for(tbb::blocked_range<size_t>(0, meshIndices.size()),
                              [&](tbb::blocked_range<size_t> r)
                              {
                                  std::vector<Ptr<Primitive>> local_vec;
                                  local_vec.reserve(r.size()/3+1);
                                  for (auto i = r.begin(); i < r.end() - 2; ++i)
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
                                      local_vec.emplace_back(std::make_shared<GeometricPrimitive>(triangle, material.get(), area_light));
                                  }
                                  std::lock_guard lck(mtx);
                                  std::copy(local_vec.begin(), local_vec.end(), std::back_inserter(_primitives));
                              });
        }
        //??????
        else
        {
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
        }

        _scene->_meshes.emplace_back(std::move(mesh));
    }

    void Parser::ParseSimpleShape(const PropertyTree &root, Transform *obj2world, Transform *world2obj)
    {
        const auto mat_string = root.GetOptional<std::string>("Material");
        Ptr<Material> material = nullptr;
        if (!mat_string || _materials.find(mat_string.get()) == _materials.end())
        {
            material = _materials["default"];
        }
        material = _materials[mat_string.get()];

        auto shape = Ptr<Shape>(static_cast<Shape *>(ObjectFactory::CreateInstance(root.Get<std::string>("Shape.Type"), root.GetChild("Shape"))));

        shape->SetTransform(obj2world, world2obj);

        auto is_emit = root.GetChildOptional("Emission");

        Ptr<AreaLight> area_light = nullptr;
        if (is_emit)
        {
            area_light = Ptr<AreaLight>(static_cast<AreaLight *>(ObjectFactory::CreateInstance("DiffuseAreaLight", is_emit.get())));
        }

        _primitives.emplace_back(std::make_shared<GeometricPrimitive>(shape, material.get(), area_light));
    }

    void Parser::ParseTransform(const PropertyTree &transform_node, Transform *obj2world)
    {
        std::vector<Transform> transforms;
        for (const auto &trans : transform_node.GetNode())
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
                //???????????????????????????
                auto iter = _rotate_node.begin();
                float theta = iter->second.get_value<float>(0.f);
                iter++;
                //????????????????????????
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

    void Parser::ParseObject(const PropertyTree &root)
    {

        for (const auto &p : root.GetChild("Object").GetNode())
        {

            //??????transform
            auto obj2world = std::make_unique<Transform>();
            auto world2obj = std::make_unique<Transform>();

            //????????????????????????value???p???(,value)??????p.second?????????
            auto _transform_node = p.second.get_child_optional("Transform");
            if (_transform_node)
            {
                ParseTransform(_transform_node.get(), obj2world.get());
            }
            (*world2obj) = Inverse(*obj2world);

            //??????Shape
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