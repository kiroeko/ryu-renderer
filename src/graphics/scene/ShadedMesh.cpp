#include "glad/gl.h"
#include "graphics/scene/ShadedMesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <iostream>
#include <filesystem>

#include "graphics/ShaderManager.h"

namespace RyuRenderer::Graphics::Scene
{
    ShadedMesh::ShadedMesh(
        const aiMesh* m,
        const aiMaterial* material,
        const std::string& textureFileRootPath,
        const DirectionalLight* directionLightPtr,
        const std::vector<PointLight>* pointLightsPtr,
        const std::vector<SpotLight>* spotLightsPtr
    )
    {
        if (!m)
        {
            std::cerr << "Model mesh data is null." << std::endl;
            return;
        }
        if (!material)
        {
            std::cerr << "Model material data is null." << std::endl;
            return;
        }
        if (!directionLightPtr)
        {
            std::cerr << "Model direction light data is null." << std::endl;
            return;
        }
        if (textureFileRootPath.empty())
        {
            std::cerr << "Model texture path is null." << std::endl;
            return;
        }
        if (!pointLightsPtr)
        {
            std::cerr << "Model point lights data is null." << std::endl;
            return;
        }
        if (!spotLightsPtr)
        {
            std::cerr << "Model spot lights data is null." << std::endl;
            return;
        }

        // load mesh
        std::vector<GLuint> indices;
        for (unsigned int i = 0; i < m->mNumFaces; ++i)
        {
            const auto& face = m->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }
        std::vector<std::array<float, 3>> positions;
        std::vector<std::array<float, 3>> normals;
        std::vector<std::array<float, 2>> texCoords;
        for (unsigned int i = 0; i < m->mNumVertices; ++i)
        {
            if (!m->HasNormals())
            {
                std::cerr << "Model mesh data has no normals." << std::endl;
                return;
            }
            if (!(m->mTextureCoords[0]))
            {
                std::cerr << "Model mesh data has no texture coords." << std::endl;
                return;
            }

            const auto& v = m->mVertices[i];
            positions.push_back({ v.x, v.y, v.z });
            const auto& n = m->mNormals[i];
            normals.push_back({ n.x, n.y, n.z });
            const auto& t = m->mTextureCoords[0][i];
            texCoords.push_back({ t.x, t.y });
        }
        mesh = Mesh(indices, positions, normals, texCoords);
        if (!mesh.IsValid())
        {
            std::cerr << "Model mesh data is invaild." << std::endl;
            return;
        }

        directionLight = directionLightPtr;
        pointLights = pointLightsPtr;
        spotLights = spotLightsPtr;

        // load material
        diffuse = GetTexture(material, aiTextureType_DIFFUSE, textureFileRootPath);
        if (!diffuse)
        {
            std::cerr << "Model diffuse texture data is invaild." << std::endl;
            return;
        }
        specular = GetTexture(material, aiTextureType_SPECULAR, textureFileRootPath);
        if (!specular)
        {
            std::cerr << "Model specular texture data is invaild." << std::endl;
            return;
        }

        materialShader = Graphics::ShaderManager::GetInstance().FindOrCreate("res/shaders/3d-blinn-phong-material.vert", "res/shaders/3d-blinn-phong-material.frag");
        if (!materialShader)
        {
            std::cerr << "Can not handle with model material shader." << std::endl;
            return;
        }
    }

    void ShadedMesh::Draw(const glm::mat4& view, const glm::mat4& projection) const
    {
        if (!IsVaild())
            return;

        diffuse->Use();
        specular->Use();
        materialShader->Use();

        materialShader->SetUniform("view", view);
        materialShader->SetUniform("projection", projection);
        materialShader->SetUniform("model", model);
        glm::mat3 viewNormalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));
        materialShader->SetUniform("viewNormalMatrix", viewNormalMatrix);
        materialShader->SetUniform("material.ambient", ambient);
        materialShader->SetUniform("material.diffuse", 0);
        materialShader->SetUniform("material.specular", 1);
        materialShader->SetUniform("material.emission", 2);
        materialShader->SetUniform("material.shininess", shininess);

        // set directional light
        materialShader->SetUniform("directionalLight.color", directionLight->Color);
        materialShader->SetUniform("directionalLight.viewDirection", glm::transpose(glm::inverse(glm::mat3(view))) * directionLight->Transformer.GetFrontDirection());

        // set point lights
        size_t pointLightCount = pointLights->size();
        constexpr size_t maxPointLightCount = 32;
        pointLightCount = pointLightCount > maxPointLightCount ? 32 : pointLightCount;
        materialShader->SetUniform("activePointLightCount", (int)pointLightCount);
        for (size_t i = 0; i < pointLightCount; ++i)
        {
            auto& l = pointLights->at(i);

            std::string pointPrefix = "pointLights[" + std::to_string(i) + "].";
            materialShader->SetUniform(pointPrefix + "color", l.Color);
            materialShader->SetUniform(pointPrefix + "viewPos", glm::vec3(view * glm::vec4(l.Transformer.GetPosition(), 1.0f)));
            materialShader->SetUniform(pointPrefix + "attenuationConstant", l.AttenuationConstant);
            materialShader->SetUniform(pointPrefix + "attenuationLinear", l.AttenuationLinear);
            materialShader->SetUniform(pointPrefix + "attenuationQuadratic", l.AttenuationQuadratic);
        }

        // set spot light
        size_t spotLightCount = spotLights->size();
        constexpr size_t maxSpotLightCount = 32;
        spotLightCount = spotLightCount > maxSpotLightCount ? 32 : spotLightCount;
        materialShader->SetUniform("activeSpotLightCount", (int)spotLightCount);
        for (size_t i = 0; i < spotLightCount; ++i)
        {
            auto& l = spotLights->at(i);

            std::string spotPrefix = "spotLights[" + std::to_string(i) + "].";
            materialShader->SetUniform(spotPrefix + "color", l.Color);
            materialShader->SetUniform(spotPrefix + "viewPos", glm::vec3(view * glm::vec4(l.Transformer.GetPosition(), 1.0f)));
            materialShader->SetUniform(spotPrefix + "viewDirection", glm::transpose(glm::inverse(glm::mat3(view))) * l.Transformer.GetFrontDirection());
            materialShader->SetUniform(spotPrefix + "innerCutOffCos", l.InnerCutOffCos);
            materialShader->SetUniform(spotPrefix + "outerCutOffCos", l.OuterCutOffCos);
            materialShader->SetUniform(spotPrefix + "attenuationConstant", l.AttenuationConstant);
            materialShader->SetUniform(spotPrefix + "attenuationLinear", l.AttenuationLinear);
            materialShader->SetUniform(spotPrefix + "attenuationQuadratic", l.AttenuationQuadratic);
        }

        // Draw
        mesh.Draw();
    }

    bool ShadedMesh::IsVaild() const
    {
        return mesh.IsValid() &&
            directionLight &&
            pointLights &&
            spotLights &&
            diffuse &&
            specular &&
            materialShader;
    }

    std::shared_ptr<Graphics::Texture2d> ShadedMesh::GetTexture(
        const aiMaterial* mat, aiTextureType t, const std::string& textureFileRootPath) const
    {
        if (!mat)
            return nullptr;
        if (t == aiTextureType_NONE)
            return nullptr;
        if (textureFileRootPath.empty())
            return nullptr;
        if (mat->GetTextureCount(t) <= 0)
            return nullptr;

        aiString textureFileName;
        mat->GetTexture(t, 0, &textureFileName);
        if (textureFileName.length == 0)
            return nullptr;
        std::string textureFileNameStr = textureFileName.C_Str();
        std::filesystem::path fullPath = std::filesystem::path(textureFileRootPath).append(textureFileNameStr);

        return TextureManager::GetInstance().FindOrCreate2d(fullPath.string(), GetTextureUnitIdxByType(t));
    }

    GLint ShadedMesh::GetTextureUnitIdxByType(aiTextureType t)
    {
        if (textureTypeUnitIdxMap.find(t) == textureTypeUnitIdxMap.end())
            return 0;
        return textureTypeUnitIdxMap[t];
    }
}