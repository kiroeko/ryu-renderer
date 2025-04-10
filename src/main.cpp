#include "App/app.h"
#include "app/pipeline/DefaultPipeline.h"

using namespace RyuRenderer::App;
using namespace RyuRenderer::App::Pipeline;

int main()
{
    if (!App::GetInstance().Init(AppSettings()))
        return -1;

    DefaultPipeline p;
    App::GetInstance().Run(&p);
    return 0;
}