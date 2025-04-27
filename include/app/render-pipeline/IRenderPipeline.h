#ifndef __IRENDERPIPELINE_H__
#define __IRENDERPIPELINE_H__

namespace RyuRenderer::App::RenderPipeline
{
    class IRenderPipeline
    {
    public:
        virtual void Init() = 0;
        virtual void Tick(double deltaTimeInS) = 0;
    };
}

#endif