#include "graphics/scene/IMaterial.h"

namespace RyuRenderer::Graphics::Scene
{
    void IMaterial::Use() const
    {
        if (!IsVaild())
            return;

        shader->Use();
    }

    bool IMaterial::IsVaild() const
    {
        if (!shader ||
            !shader->IsValid())
            return false;
        return true;
    }

    std::string IMaterial::GetName() const
    {
        return name;
    }
}