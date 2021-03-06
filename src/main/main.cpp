

#include <core/parser.h>
#include <chrono>
#include <ROOT_PATH.h>
//"D:/Homework/graphics/rendering/Platinum/assets/scene/cornellbox-sphere.json"
using namespace platinum;
using namespace std;

const static string root_path(ROOT_PATH);
const static string log_info_path = root_path + "/logs/info";
const static string log_warning_path = root_path + "/logs/warning";
const static string log_error_path = root_path + "/logs/error";



int main(int argc, char *argv[])
{
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::GLOG_INFO, log_info_path.c_str());
    google::SetLogDestination(google::GLOG_WARNING, log_warning_path.c_str());
    google::SetLogDestination(google::GLOG_ERROR, log_info_path.c_str());
    //第一个参数argv[0]一定是程序的名称
    auto &parser = Parser::GetInstance();
    if (argc >= 2)
    {
        Ptr<Integrator> integrator = nullptr;
        Ptr<Scene> scene = nullptr;
        std::string filename(argv[1]);
        
        parser.Parse(filename, scene, integrator);



        integrator->Render(*scene);

    }

    google::ShutdownGoogleLogging();

    return 0;

  
}
