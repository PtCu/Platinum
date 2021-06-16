#include "parser.h"

#include "film.h"
#include "sampler.h"
#include "filter.h"
#include "shape.h"
#include "camera.h"
#include "material.h"
#include "light.h"
#include "accelerators/kd_tree.h"
#include "accelerators/linear_aggregate.h"
#include <fstream>

using namespace nlohmann;
namespace platinum
{
    void Parser::parser(const std::string &path, Scene::ptr &_scene, Integrator::ptr &_integrator)
    {
        _integrator = nullptr;
        _scene = nullptr;

        json _scene_json;
        {
            std::ifstream infile(path);

            if (!infile)
            {
                LOG(ERROR) << "Could not open the json file: " << path;
            }

            infile >> _scene_json;
            infile.close();
        }
        LOG(INFO) << "Parse the scene file from " << path;

        //根据json数据类型来返回对应的值
        auto get_func = [](const json_value_type &target) -> std::string {
            if (target.is_string())
            {
                return target.get<std::string>();
            }
            else if (target.is_boolean())
            {
                bool ret = target.get<bool>();
                return ret ? "true" : "false";
            }
            else if (target.is_number_float())
            {
                float ret = target.get<float>();
                std::stringstream ss;
                ss << ret;
                return ss.str();
            }
            else
            {
                int ret = target.get<int>();
                std::stringstream ss;
                ss << ret;
                return ss.str();
            }
        };

        //Setup directory path
        {
            size_t last_slash_idx = path.rfind('\\');
            //搜寻各种格式
            if (last_slash_idx == std::string::npos)
            {
                last_slash_idx = path.rfind('/');
            }
            if (last_slash_idx != std::string::npos)
            {
                PropertyTreeNode::m_directory = path.substr(0, last_slash_idx + 1);
            }
        }

        //Build property tree
        std::function<PropertyTreeNode(const std::string &tag, const json_value_type &jsonData)> build_property_tree_func;
        build_property_tree_func = [&](const std::string &tag, const json_value_type &jsonData) -> PropertyTreeNode {
            PropertyTreeNode node(tag);
            if (jsonData.is_object())
            {
                for (const auto &item : jsonData.items())
                {
                    const auto &key = item.key();
                    const auto &value = item.value();
                    if (!value.is_object())
                    {
                        std::vector<std::string> values;
                        if (value.is_array())
                        {
                            for (int i = 0; i < value.size(); ++i)
                            {
                                values.push_back(get_func(value[i]));
                            }
                        }
                        else
                        {
                            values.push_back(get_func(value));
                        }
                        node.addProperty(key, values);
                    }
                    else
                    {
                        //build-in class type, recursively build it
                        node.addChild(build_property_tree_func(key, jsonData[key]));
                    }
                }
            }
            return node;
        };

        //Integrator loading
        {
            if (!_scene_json.contains("Integrator"))
            {
                LOG(ERROR) << "There is no Integrator in " << path;
            }
            PropertyTreeNode integratorNode = build_property_tree_func("Integrator", _scene_json["Integrator"]);
            _integrator = Integrator::ptr(static_cast<Integrator *>(ObjectFactory::createInstance(
                integratorNode.getTypeName(), integratorNode)));
        }

        std::vector<Light::ptr> _lights;
        std::vector<Entity::ptr> _entities;
        std::vector<Hitable::ptr> _hitables;

        //Entity loading
        {
            if (!_scene_json.contains("Entity"))
            {
                LOG(ERROR) << "There is no Entity in " << path;
            }
            const auto &entities_json = _scene_json["Entity"];
            for (int i = 0; i < entities_json.size(); ++i)
            {
                PropertyTreeNode entityNode = build_property_tree_func("Entity", entities_json[i]);
                Entity::ptr entity = Entity::ptr(static_cast<Entity *>(ObjectFactory::createInstance(
                    entityNode.getTypeName(), entityNode)));
                _entities.push_back(entity);
            }

            for (auto &entity : _entities)
            {
                for (const auto &hitable : entity->getHitables())
                {
                    _hitables.push_back(hitable);
                }
            }

            for (int i = 0; i < _hitables.size(); ++i)
            {
                if (_hitables[i]->getAreaLight() != nullptr)
                {
                    _lights.push_back(Light::ptr(dynamic_cast<HitableObject *>(_hitables[i].get())->getAreaLightPtr()));
                }
            }
        }

        KdTree::ptr _aggregate = std::make_shared<KdTree>(_hitables);
        //LinearAggregate::ptr _aggregate = std::make_shared<LinearAggregate>(_hitables);
        _scene = std::make_shared<Scene>(_entities, _aggregate, _lights);
    }

}