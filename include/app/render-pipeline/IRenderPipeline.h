#ifndef __IRENDERPIPELINE_H__
#define __IRENDERPIPELINE_H__

namespace RyuRenderer::App::RenderPipeline
{
    class IRenderPipeline
    {
    public:
        virtual void init() = 0;
        virtual void tick(double deltaTimeInS) = 0;
    };
}

#endif