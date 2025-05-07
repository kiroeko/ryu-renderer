#include "graphics/scene/Scene.h"

#include <filesystem>
#include <iostream>
#include <typeinfo>

#include "graphics/ShaderManager.h"
#include "graphics/TextureManager.h"
#include "graphics/scene/IMaterial.h"
#include "graphics/scene/PhongBlinnMaterial.h"
#include "graphics/scene/Transform.h"
#include "graphics/scene/MeshObject.h"

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

        Transform defaultTransformer;
        for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
        {
            aiMesh* mesh = scene->mMeshes[i];
            if (!mesh ||
                mesh->mMaterialIndex < 0)
                continue;

            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            if (!material)
                continue;

            // load mesh
            std::vector<GLuint> indices;
            for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
            {
                const auto& face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; ++j)
                    indices.push_back(face.mIndices[j]);
            }
            std::vector<std::array<float, 3>> positions;
            std::vector<std::array<float, 3>> normals;
            std::vector<std::array<float, 2>> texCoords;
            bool isMeshVaild = true;
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
            {
                if (!mesh->HasNormals())
                {
                    std::cerr << "Model mesh data has no normals." << std::endl;
                    isMeshVaild = false;
                    break;
                }
                if (!(mesh->mTextureCoords[0]))
                {
                    std::cerr << "Model mesh data has no texture coords." << std::endl;
                    isMeshVaild = false;
                    break;
                }

                const auto& v = mesh->mVertices[i];
                positions.push_back({ v.x, v.y, v.z });
                const auto& n = mesh->mNormals[i];
                normals.push_back({ n.x, n.y, n.z });
                const auto& t = mesh->mTextureCoords[0][i];
                texCoords.push_back({ t.x, t.y });
            }
            if (!isMeshVaild)
                continue;

            Mesh m = Mesh(indices, positions, normals, texCoords);
            if (!m.IsValid())
            {
                std::cerr << "Model mesh data is invaild." << std::endl;
                continue;
            }

            // load material
            std::string trp = textureFileRootPath.string();
            auto diffuse = GetTexture(material, aiTextureType_DIFFUSE, trp);
            if (!diffuse)
            {
                std::clog << "Model diffuse texture data is invaild." << std::endl;
                continue;
            }
            auto specular = GetTexture(material, aiTextureType_SPECULAR, trp);
            if (!specular)
            {
                std::clog << "Model specular texture data is invaild." << std::endl;
            }
            auto emission = GetTexture(material, aiTextureType_EMISSIVE, trp);
            if (!emission)
            {
                std::clog << "Model emission texture data is invaild." << std::endl;
            }

            // Build dynamic material
            const type_info* materialType = nullptr;
            std::shared_ptr<IMaterial> newMaterial = nullptr;
            std::any materialData;
            if (diffuse)
            {
                materialType = &typeid(PhongBlinnMaterial);

                newMaterial = std::make_shared<PhongBlinnMaterial>();

                PhongBlinnMaterialData d = PhongBlinnMaterialData();
                d.Diffuse = diffuse;
                d.Specular = specular;
                d.Emission = emission;
                d.DirectionLight = &DirectionLight;
                d.PointLights = &PointLights;
                d.SpotLights = &SpotLights;
                materialData = d;

                newMaterial->SetData(materialData);
            }
            if (!materialType ||
                !newMaterial)
                continue;

            bool isBatchMatch = false;
            for (auto& mb : meshObjectBatches)
            {
                if (!mb.IsVaild())
                    continue;
                if (!mb.Match(*materialType))
                    continue;

                isBatchMatch = true;

                bool isObjectMatch = false;
                for (auto& mo : mb.MeshObjects)
                {
                    if (mo.MaterialData.type() == typeid(PhongBlinnMaterialData) &&
                        materialData.type() == typeid(PhongBlinnMaterialData))
                    {
                        auto md0 = std::any_cast<PhongBlinnMaterialData>(materialData);
                        auto md1 = std::any_cast<PhongBlinnMaterialData>(mo.MaterialData);

                        if (md0 != md1)
                            continue;
                    }
                    else
                    {
                        continue;
                    }

                    if (mo.Transformer == defaultTransformer)
                    {
                        mo.Meshes.emplace_back(std::move(m));
                        isObjectMatch = true;
                    }
                }

                if (!isObjectMatch)
                {
                    MeshObject tmo;
                    tmo.Meshes.emplace_back(std::move(m));
                    tmo.Transformer = defaultTransformer;
                    tmo.MaterialData = materialData;
                    mb.MeshObjects.emplace_back(std::move(tmo));
                }
                break;
            }

            if (!isBatchMatch)
            {
                MeshObjectBatch tmob(newMaterial);
                MeshObject tmo;
                tmo.Meshes.emplace_back(std::move(m));
                tmo.Transformer = defaultTransformer;
                tmo.MaterialData = materialData;
                tmob.MeshObjects.emplace_back(std::move(tmo));
                meshObjectBatches.emplace_back(std::move(tmob));
            }
        }

        return true;
    }

    void Scene::Draw() const
    {
        glm::mat4 view = Camera.GetView();
        glm::mat4 projection = Camera.GetProjection();

        /// draw lights
        lightShader->Use();
        lightShader->SetUniform("view", view);
        lightShader->SetUniform("projection", projection);

        // Point lights
        for (const auto& l : PointLights)
        {
            lightShader->SetUniform("model", l.Transformer.GetMatrix());
            lightShader->SetUniform("color", l.Color);

            for (const auto& lm : lightMeshes)
            {
                lm.Draw();
            }
        }

        // Spot lights
        for (const auto& l : SpotLights)
        {
            lightShader->SetUniform("model", l.Transformer.GetMatrix());
            lightShader->SetUniform("color", l.Color);

            for (const auto& lm : lightMeshes)
            {
                lm.Draw();
            }
        }

        /// Draw mesh batches
        for (auto& o : meshObjectBatches)
        {
            if (!o.IsVaild())
                continue;

            o.Draw(view, projection);
        }
    }

    void Scene::ClearObjects()
    {
        meshObjectBatches.clear();
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

    std::shared_ptr<Graphics::Texture2d> Scene::GetTexture(
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
        if (!std::filesystem::exists(fullPath))
            return nullptr;

        return TextureManager::GetInstance().FindOrCreate2d(fullPath.string(), GetTextureUnitIdxByType(t));
    }

    GLint Scene::GetTextureUnitIdxByType(aiTextureType t)
    {
        if (textureTypeUnitIdxMap.find(t) == textureTypeUnitIdxMap.end())
            return 0;
        return textureTypeUnitIdxMap[t];
    }
}