#include "App/app.h"

using namespace RyuRenderer::App;

int main()
{
    if (!App::GetInstance().Init(AppSettings()))
        return -1;

    App::GetInstance().Run();
    return 0;
}