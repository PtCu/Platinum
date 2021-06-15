#include "renderer.h"

namespace platinum{

void Renderer::render(const std::vector<std::string> &filenames)
{
    for (const auto &f : filenames)
    {
        render(f);
    }
}

void Renderer::render(const std::string &filename)
{
    scene = nullptr;
    integrator = nullptr;
    Parser::parser(filename, scene, integrator);
    integrator->preprocess(*scene);
    integrator->render(*scene);
}

}