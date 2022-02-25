#include <core/parser.h>
#include <crtdbg.h>
using namespace platinum;
using namespace std;


int main()
{
    _CrtSetBreakAlloc(7685);
    auto& parser = Parser::GetInstance();
    Ptr<Integrator> integrator = nullptr;
    Ptr<Scene> scene = nullptr;
    std::string filename = "D:/Homework/graphics/rendering/Platinum/assets/scene/sphere.json";
    parser.Parse(filename, scene, integrator);
    integrator->Render(*scene);

    _CrtDumpMemoryLeaks();
    return 0;

}

