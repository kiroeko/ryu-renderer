#ifndef __IMATERIAL_H__
#define __IMATERIAL_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <any>
#include <memory>
#include <string>

#include "graphics/Shader.h"
#include "graphics/scene/DirectionalLight.h"
#include "graphics/scene/PointLight.h"
#include "graphics/scene/SpotLight.h"

namespace RyuRenderer::Graphics::Scene
{
    class IMaterial
    {
    public:
        virtual void Use() const;

        virtual void SetData(const std::any& d) = 0;

        virtual bool IsVaild() const;

        std::string GetName() const;
    protected:
        // Material basic info
        std::string name;

        // Shader program
        std::shared_ptr<Graphics::Shader> shader;
    };
}

#endif