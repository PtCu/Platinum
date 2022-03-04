#include <core/parser.h>
#include <crtdbg.h>
#include <ROOT_PATH.h>
using namespace platinum;
using namespace std;

const static string root_path(ROOT_PATH);
const static string log_info_path = root_path + "/logs/info";
const static string log_warning_path = root_path + "/logs/warning";
const static string log_error_path = root_path + "/logs/error";

int main(int argc, char *argv[])
{
    _CrtSetBreakAlloc(7685);
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::GLOG_INFO, log_info_path.c_str());
    google::SetLogDestination(google::GLOG_WARNING, log_warning_path.c_str());
    google::SetLogDestination(google::GLOG_ERROR, log_info_path.c_str());
    auto& parser = Parser::GetInstance();
    Ptr<Integrator> integrator = nullptr;
    Ptr<Scene> scene = nullptr;
    std::string filename = "D:/Homework/graphics/rendering/Platinum/assets/scene/bunny.json";
    parser.Parse(filename, scene, integrator);
    integrator->Render(*scene);
    google::ShutdownGoogleLogging();
    _CrtDumpMemoryLeaks();
    return 0;

}

