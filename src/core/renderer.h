#ifndef CORE_RENDERER_H_
#define CORE_RENDERER_H_

#include "core/platinum.h"
#include "core/primitive.h"

namespace platinum
{
    class Renderer
    {
    public:
        static Renderer *getInstance();
        void renderer(const std::string &f);

    private:
        static Renderer *m_renderer;

    };
}

#endif
