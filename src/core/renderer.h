#ifndef CORE_RENDERER_H_
#define CORE_RENDERER_H_

#include "platinum.h"
#include "integrator.h"
#include "parser.h"
#include "scene.h"

namespace platinum{

//TO-DO: 完善析构
class Renderer
{
public:
    static Renderer *getInstance()
    {
        if (m_renderer == NULL)
            m_renderer = new Renderer();
        return m_renderer;
    }  
    void render(const std::vector<std::string> &filenames);

private:
    Renderer();
    ~Renderer();
    Renderer(const Renderer &);
    Renderer &operator=(const Renderer &);
    void render(const std::string &filename);
    static Renderer *m_renderer;

    Scene::ptr scene;
    Integrator::ptr integrator;

};

}

#endif
