#ifndef CORE_PARSER_H_
#define CORE_PARSER_H_

#include "platinum.h"
#include "math/utils.h"
#include "integrator.h"
#include "scene.h"

#include "nlohmann/json.hpp"

PLATINUM_BEGIN

class Parser final
{
public:
    static void parser(const std::string &path, Scene::ptr &_scene, Integrator::ptr &integrator);

private:
    using json_value_type = nlohmann::basic_json<>::value_type;
};
PLATINUM_END

#endif