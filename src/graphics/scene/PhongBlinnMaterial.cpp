#include "graphics/scene/PhongBlinnMaterial.h"

#include "graphics/ShaderManager.h"
#include "graphics/scene/Scene.h"

namespace RyuRenderer::Graphics::Scene
{
    PhongBlinnMaterial::PhongBlinnMaterial()
    {
        name = "PhongBlinnMaterial";
        shader = Graphics::ShaderManager::GetInstance().FindOrCreate(
            "res/shaders/3d-blinn-phong-material.vert",
            "res/shaders/3d-blinn-phong-material.frag");
    }

    void PhongBlinnMaterial::SetData(const std::any& d)
    {
        if (d.type() != typeid(PhongBlinnMaterialData))
            return;

        data = std::any_cast<PhongBlinnMaterialData>(d);
    }

    void PhongBlinnMaterial::Use() const
    {
        if (!IsVaild())
            return;

        // Set material basic data
        if (data.Diffuse)
            data.Diffuse->Use();
        if (data.Specular)
            data.Specular->Use();
        if (data.Emission)
            data.Emission->Use();

        IMaterial::Use();

        // Set mvp
        shader->SetUniform("model", data.Model);
        shader->SetUniform("view", data.View);
        shader->SetUniform("projection", data.Projection);
        glm::mat3 viewNormalMatrix = glm::transpose(glm::inverse(glm::mat3(data.View * data.Model)));
        shader->SetUniform("viewNormalMatrix", viewNormalMatrix);

        // Set directional light
        if (data.DirectionLight)
        {
            shader->SetUniform("directionalLight.color", data.DirectionLight->Color);
            shader->SetUniform(
                "directionalLight.viewDirection",
                glm::transpose(glm::inverse(glm::mat3(data.View))) * data.DirectionLight->Transformer.GetFrontDirection());
        }

        // Set point lights
        if (data.PointLights)
        {
            size_t pointLightCount = data.PointLights->size();
            constexpr size_t maxPointLightCount = 32;
            pointLightCount = pointLightCount > maxPointLightCount ? 32 : pointLightCount;
            shader->SetUniform("activePointLightCount", (int)pointLightCount);
            for (size_t i = 0; i < pointLightCount; ++i)
            {
                auto& l = data.PointLights->at(i);

                std::string pointPrefix = "pointLights[" + std::to_string(i) + "].";
                shader->SetUniform(pointPrefix + "color", l.Color);
                shader->SetUniform(pointPrefix + "viewPos", glm::vec3(data.View * glm::vec4(l.Transformer.GetPosition(), 1.0f)));
                shader->SetUniform(pointPrefix + "attenuationConstant", l.AttenuationConstant);
                shader->SetUniform(pointPrefix + "attenuationLinear", l.AttenuationLinear);
                shader->SetUniform(pointPrefix + "attenuationQuadratic", l.AttenuationQuadratic);
            }
        }

        // Set spot light
        if (data.SpotLights)
        {
            size_t spotLightCount = data.SpotLights->size();
            constexpr size_t maxSpotLightCount = 32;
            spotLightCount = spotLightCount > maxSpotLightCount ? 32 : spotLightCount;
            shader->SetUniform("activeSpotLightCount", (int)spotLightCount);
            for (size_t i = 0; i < spotLightCount; ++i)
            {
                auto& l = data.SpotLights->at(i);

                std::string spotPrefix = "spotLights[" + std::to_string(i) + "].";
                shader->SetUniform(spotPrefix + "color", l.Color);
                shader->SetUniform(spotPrefix + "viewPos", glm::vec3(data.View * glm::vec4(l.Transformer.GetPosition(), 1.0f)));
                shader->SetUniform(spotPrefix + "viewDirection", glm::transpose(glm::inverse(glm::mat3(data.View))) * l.Transformer.GetFrontDirection());
                shader->SetUniform(spotPrefix + "innerCutOffCos", l.InnerCutOffCos);
                shader->SetUniform(spotPrefix + "outerCutOffCos", l.OuterCutOffCos);
                shader->SetUniform(spotPrefix + "attenuationConstant", l.AttenuationConstant);
                shader->SetUniform(spotPrefix + "attenuationLinear", l.AttenuationLinear);
                shader->SetUniform(spotPrefix + "attenuationQuadratic", l.AttenuationQuadratic);
            }
        }

        shader->SetUniform("material.ambient", data.Ambient);
        if (data.Diffuse)
        {
            shader->SetUniform("hasDiffuse", true);
            shader->SetUniform("material.diffuse", Scene::GetTextureUnitIdxByType(aiTextureType_DIFFUSE));
        }
        if (data.Specular)
        {
            shader->SetUniform("hasSpecular", true);
            shader->SetUniform("material.specular", Scene::GetTextureUnitIdxByType(aiTextureType_SPECULAR));
        }
        if (data.Emission)
        {
            shader->SetUniform("hasEmission", true);
            shader->SetUniform("material.emission", Scene::GetTextureUnitIdxByType(aiTextureType_EMISSIVE));
        }
        shader->SetUniform("material.shininess", data.Shininess);
    }
}