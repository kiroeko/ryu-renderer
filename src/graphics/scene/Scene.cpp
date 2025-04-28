#include "graphics/scene/Scene.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <filesystem>
#include <iostream>

#include "graphics/ShaderManager.h"

namespace RyuRenderer::Graphics::Scene
{
    Scene::Scene()
    {
        // init meshes
        lightMeshes.emplace_back(Graphics::Mesh(
            std::vector<GLuint>{
                // front
                0, 1, 2,
                2, 3, 0,
                // back
                4, 5, 6,
                6, 7, 4,
                // left
                0, 3, 7,
                7, 4, 0,
                // right
                1, 5, 6,
                6, 2, 1,
                // down
                0, 1, 5,
                5, 4, 0,
                // up
                3, 2, 6,
                6, 7, 3
            }, // indexes
            std::vector<std::array<float, 3>>{
                {  -0.1f, -0.1f, -0.1f },
                { 0.1f, -0.1f, -0.1f },
                { 0.1f,  0.1f, -0.1f },
                { -0.1f,  0.1f, -0.1f },
                { -0.1f, -0.1f,  0.1f },
                { 0.1f, -0.1f, 0.1f },
                { 0.1f, 0.1f, 0.1f },
                { -0.1f,  0.1f,  0.1f },
            } // Position
        ));

        // init shaders
        lightShader = Graphics::ShaderManager::GetInstance().FindOrCreate("res/shaders/3d-basic-color.vert", "res/shaders/3d-basic-color.frag");
    }

    bool Scene::Load(const std::string& modelFilePath)
    {
        if (!std::filesystem::exists(modelFilePath))
            return false;
        const auto p = std::filesystem::path(modelFilePath);
        const auto textureFileRootPath = p.parent_path();
        if (!std::filesystem::exists(textureFileRootPath) ||
            !std::filesystem::is_directory(textureFileRootPath)) {
            return false;
        }

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            modelFilePath,
            aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if (!scene ||
            scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
            !scene->mMeshes)
        {
            std::cerr << "Assimp load file error: " << importer.GetErrorString() << std::endl;
            return false;
        }
        if (scene->mNumMeshes <= 0)
            return false;

        for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
        {
            aiMesh* mesh = scene->mMeshes[i];
            if (!mesh ||
                mesh->mMaterialIndex < 0)
                continue;

            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            if (!material)
                continue;

            objectMeshes.emplace_back(
                ShadedMesh(
                    mesh,
                    material,
                    textureFileRootPath.string(),
                    &DirectionLight,
                    &PointLights,
                    &SpotLights
                )
            );
        }

        return true;
    }

    void Scene::Draw() const
    {
        glm::mat4 view = Camera.GetView();
        glm::mat4 projection = Camera.GetProjection();

        // draw light
        lightShader->Use();
        lightShader->SetUniform("view", view);
        lightShader->SetUniform("projection", projection);

        // point lights
        for (const auto& l : PointLights)
        {
            lightShader->SetUniform("model", l.GetModel());
            lightShader->SetUniform("color", l.Color);

            for (const auto& lm : lightMeshes)
            {
                lm.Draw();
            }
        }

        // spot lights
        for (const auto& l : SpotLights)
        {
            lightShader->SetUniform("model", l.GetModel());
            lightShader->SetUniform("color", l.Color);

            for (const auto& lm : lightMeshes)
            {
                lm.Draw();
            }
        }

        // draw objects
        for (const auto& o : objectMeshes)
        {
            o.Draw(view, projection);
        }
    }

    void Scene::ClearObjects()
    {
        objectMeshes.clear();
    }

    void Scene::OnTick(double deltaTimeInS)
    {
        Camera.OnTick(deltaTimeInS);
    }

    void Scene::OnWindowResize(float aspectRatio)
    {
        Camera.SetAspectRatio(aspectRatio);
    }

    void Scene::OnMouseMove(const App::Events::MouseEvent& e)
    {
        Camera.OnMouseMove(e);
    }

    void Scene::OnKeyEvent(const App::Events::KeyEvent& e)
    {
        Camera.OnKeyEvent(e);
    }

    glm::vec3 Scene::GetNegativeZAxisDirection()
    {
        return glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::vec3 Scene::GetZAxisDirection()
    {
        return glm::vec3(0.0f, 0.0f, 1.0f);
    }

    glm::vec3 Scene::GetNegativeXAxisDirection()
    {
        return glm::vec3(-1.0f, 0.0f, 0.0f);
    }

    glm::vec3 Scene::GetXAxisDirection()
    {
        return glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glm::vec3 Scene::GetYAxisDirection()
    {
        return glm::vec3(0.0f, 1.0f, 0.0f);
    }

    glm::vec3 Scene::GetNegativeYAxisDirection()
    {
        return glm::vec3(0.0f, -1.0f, 0.0f);
    }

    glm::vec3 Scene::GetFrontDirection()
    {
        return GetNegativeZAxisDirection();
    }

    glm::vec3 Scene::GetBackDirection()
    {
        return GetZAxisDirection();
    }

    glm::vec3 Scene::GetLeftDirection()
    {
        return GetNegativeXAxisDirection();
    }

    glm::vec3 Scene::GetRightDirection()
    {
        return GetXAxisDirection();
    }

    glm::vec3 Scene::GetUpDirection()
    {
        return GetYAxisDirection();
    }

    glm::vec3 Scene::GetDownDirection()
    {
        return GetNegativeYAxisDirection();
    }
}