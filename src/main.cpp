#include "App/app.h"
#include "app/render-pipeline/DefaultRenderPipeline.h"

using namespace RyuRenderer::App;
using namespace RyuRenderer::App::RenderPipeline;

int main()
{
    if (!App::GetInstance().Init(AppSettings()))
        return -1;

    DefaultRenderPipeline p;
    App::GetInstance().Run(&p);
    return 0;
}