#include <core/parser.h>
#include <material/mirror.h>
#include <gtest/gtest.h>
using namespace platinum;
using namespace std;


void RenderFromJson()
{
    auto& parser = Parser::GetInstance();
    Ptr<Integrator> integrator = nullptr;
    Ptr<Scene> scene = nullptr;
    std::string filename = "D:/Homework/graphics/rendering/Platinum/assets/scene/cornellbox.json";
    parser.Parse(filename, scene, integrator);
    integrator->Render(*scene);

}
