#include <core/parser.h>

using namespace platinum;
using namespace std;
int main(int argc, char **argv)
{
    auto parser = Parser::GetInstance();
    Ptr<Integrator> integrator = nullptr;
    Ptr<Scene> scene = nullptr;
    std::string filename = "D:/Homework/graphics/rendering/Platinum/assets/scene/cornellbox.json";
    parser.Parse(filename, scene, integrator);
    integrator->Render(*scene);

#ifdef _MSC_VER
        system("pause");
#endif
    return 0;
}
