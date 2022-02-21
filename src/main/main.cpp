// Copyright 2022 ptcup
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
    // google::InitGoogleLogging(argv[0]);
    // google::SetLogDestination(google::GLOG_INFO, log_info_path.c_str());
    // google::SetLogDestination(google::GLOG_WARNING, log_warning_path.c_str());
    // google::SetLogDestination(google::GLOG_ERROR, log_info_path.c_str());
    // //第一个参数argv[0]一定是程序的名称
    // auto &parser = Parser::GetInstance();
    // if (argc >= 2)
    // {
    //     Ptr<Integrator> integrator = nullptr;
    //     Ptr<Scene> scene = nullptr;
    //     std::string filename(argv[1]);
    //     auto t0 = chrono::steady_clock::now();
    //     parser.Parse(filename, scene, integrator);
    //     auto t1 = chrono::steady_clock::now();
    //     auto dt = t1 - t0;
    //     LOG(INFO) << " Parsing time: " << chrono::duration_cast<chrono::milliseconds>(dt).count() << "ms";

    //     t0 = chrono::steady_clock::now();
    //     integrator->Render(*scene);
    //     t1 = chrono::steady_clock::now();
    //     dt = t1 - t0;
    //     LOG(INFO) << " Rendering time: " << chrono::duration_cast<chrono::milliseconds>(dt).count()<<"ms";
    // }

    // google::ShutdownGoogleLogging();

    // return 0;
    auto &parser = Parser::GetInstance();
    Ptr<Integrator> integrator = nullptr;
    Ptr<Scene> scene = nullptr;
    std::string filename = "D:/Homework/graphics/rendering/Platinum/assets/scene/cornellbox-sphere.json";
    parser.Parse(filename, scene, integrator);
    integrator->Render(*scene);
}
