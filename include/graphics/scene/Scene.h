#ifndef __SCENE_H__
#define __SCENE_H__

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glm/glm.hpp"

#include <list>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

#include "app/events/MouseEvent.h"
#include "app/events/KeyEvent.h"
#include "graphics/Mesh.h"
#include "graphics/Shader.h"
#include "graphics/Texture2d.h"
#include "graphics/scene/Camera.h"
#include "graphics/scene/DirectionalLight.h"
#include "graphics/scene/PointLight.h"
#include "graphics/scene/SpotLight.h"
#include "graphics/scene/MeshObjectBatch.h"

namespace RyuRenderer::Graphics::Scene
{
    class Scene
    {
    public:
        enum SceneMaterialTypeEnum {
            AUTO = 0,
            PBR = 1,
            PHONG_BLINN = 2
        };

        Scene() = default;

        bool Load(
            const std::string& modelFilePath,
            bool isFilpUVs = true,
            SceneMaterialTypeEnum sceneMaterialType = SceneMaterialTypeEnum::AUTO);

        void Draw() const;

        void ClearObjects();

        void OnTick(double deltaTimeInS);

        void OnWindowResize(float aspectRatio);

        void OnMouseMove(const App::Events::MouseEvent& e);

        void OnKeyEvent(const App::Events::KeyEvent& e);

        static glm::vec3 GetNegativeZAxisDirection();

        static glm::vec3 GetZAxisDirection();

        static glm::vec3 GetNegativeXAxisDirection();

        static glm::vec3 GetXAxisDirection();

        static glm::vec3 GetYAxisDirection();

        static glm::vec3 GetNegativeYAxisDirection();

        static glm::vec3 GetFrontDirection();

        static glm::vec3 GetBackDirection();

        static glm::vec3 GetLeftDirection();

        static glm::vec3 GetRightDirection();

        static glm::vec3 GetUpDirection();

        static glm::vec3 GetDownDirection();

        static GLint GetTextureUnitIdxByType(aiTextureType t);

        Camera Camera;

        DirectionalLight DirectionLight;
        std::vector<PointLight> PointLights;
        std::vector<SpotLight> SpotLights;

        std::list<MeshObjectBatch> MeshObjectBatches;

        std::list<Graphics::Mesh> LightMeshes;
        std::shared_ptr<Graphics::Shader> LightShader = nullptr;
    private:
        void LoadMaterial(
            const SceneMaterialTypeEnum& sceneMaterialType,
            const aiMaterial* const material,
            const std::string& textureFileRootPath,
            const type_info*& outMaterialType,
            std::any& outMaterialData,
            std::shared_ptr<IMaterial>& outNewMaterial);

        void LoadMaterialPBR(
            const aiMaterial* const material,
            const std::string& textureFileRootPath,
            const type_info*& outMaterialType,
            std::any& outMaterialData,
            std::shared_ptr<IMaterial>& outNewMaterial);

        void LoadMaterialPhongBlinn(
            const aiMaterial* const material,
            const std::string& textureFileRootPath,
            const type_info*& outMaterialType,
            std::any& outMaterialData,
            std::shared_ptr<IMaterial>& outNewMaterial);

        std::shared_ptr<Graphics::Texture2d> GetTexture(
            const aiMaterial* mat, aiTextureType t, const std::string& textureFileRootPath) const;

        inline static std::unordered_map<aiTextureType, GLint> textureTypeUnitIdxMap = {
            { aiTextureType_DIFFUSE, 0 },
            { aiTextureType_SPECULAR, 1 },
            { aiTextureType_EMISSIVE, 2 },
            { aiTextureType_BASE_COLOR, 3 },
            { aiTextureType_METALNESS, 4 },
            { aiTextureType_DIFFUSE_ROUGHNESS, 5 },
            { aiTextureType_AMBIENT_OCCLUSION, 6 }
        };
    };
}

#endif