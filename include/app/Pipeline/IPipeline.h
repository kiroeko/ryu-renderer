#ifndef __IPIPELINE_H__
#define __IPIPELINE_H__

namespace RyuRenderer::App::Pipeline
{
    class IPipeline
    {
    public:
        virtual void init() = 0;
        virtual void tick() = 0;
    };
}

#endif