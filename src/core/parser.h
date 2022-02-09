#ifndef CORE_PARSER_H_
#define CORE_PARSER_H_

#include <boost/property_tree/json_parser.hpp>

#include <core/object.h>
#include <core/utilities.h>
#include <core/integrator.h>
#include <core/scene.h>
#include <core/camera.h>
#include <core/sampler.h>
#include <core/film.h>

namespace platinum
{

    class Parser final
    {
    public:
        static void Parse(const std::string &path,Ptr<Scene>scene,Ptr<Integrator> integrator);
    };
}
#endif