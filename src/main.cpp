#include "App/app.h"
#include "app/render-pipeline/StencilDepthPhongBlinnPipeline.h"

using namespace RyuRenderer::App;
using namespace RyuRenderer::App::RenderPipeline;

int main()
{
    if (!App::GetInstance().Init(AppSettings()))
        return -1;

    StencilDepthPhongBlinnPipeline p;
    App::GetInstance().Run(&p);
    return 0;
}