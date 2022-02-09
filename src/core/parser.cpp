#include <core/parser.h>

#include <fstream>
#include <exception>

namespace platinum
{

    void Parser::Parse(const std::string &path, Ptr<Scene> scene, Ptr<Integrator> integrator)
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

  
     
    }
}