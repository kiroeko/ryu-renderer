#ifndef __SHADEDMESH_H__
#define __SHADEDMESH_H__

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

#include "graphics/Mesh.h"
#include "graphics/Shader.h"
#include "graphics/Texture2d.h"
#include "graphics/TextureManager.h"
#include "graphics/scene/DirectionalLight.h"
#include "graphics/scene/PointLight.h"
#include "graphics/scene/SpotLight.h"

namespace RyuRenderer::Graphics::Scene
{
    class ShadedMesh
    {
    public:
        ShadedMesh(
            const aiMesh* m,
            const aiMaterial* material,
            const std::string& textureFileRootPath,
            const DirectionalLight* directionLightPtr,
            const std::vector<PointLight>* pointLightsPtr,
            const std::vector<SpotLight>* spotLightsPtr
        );

        void Draw(const glm::mat4& view, const glm::mat4& projection) const;

        bool IsVaild() const;
    private:
        std::shared_ptr<Graphics::Texture2d> GetTexture(
            const aiMaterial* mat, aiTextureType t, const std::string& textureFileRootPath) const;

        static GLint GetTextureUnitIdxByType(aiTextureType t);

        Graphics::Mesh mesh;

        const DirectionalLight* directionLight = nullptr;
        const std::vector<PointLight>* pointLights = nullptr;
        const std::vector<SpotLight>* spotLights = nullptr;

        glm::vec3 ambient = { 0.2f, 0.2f, 0.2f };
        std::shared_ptr<Graphics::Texture2d> diffuse;
        std::shared_ptr<Graphics::Texture2d> specular;
        float shininess = 128.f;

        std::shared_ptr<Graphics::Shader> materialShader;

        glm::mat4 model = glm::identity<glm::mat4>();

        inline static std::unordered_map<aiTextureType, GLint> textureTypeUnitIdxMap = {
            { aiTextureType_DIFFUSE, 0 },
            { aiTextureType_SPECULAR, 1 }
        };
    };
}

#endif