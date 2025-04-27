#include "App/app.h"
#include "app/render-pipeline/ModelViewPhongBlinnPipeline.h"

using namespace RyuRenderer::App;
using namespace RyuRenderer::App::RenderPipeline;

int main()
{
    if (!App::GetInstance().Init(AppSettings()))
        return -1;

    ModelViewPhongBlinnPipeline p;
    App::GetInstance().Run(&p);
    return 0;
}