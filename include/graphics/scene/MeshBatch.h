#ifndef __MESHBATCH_H__
#define __MESHBATCH_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <any>
#include <list>
#include <memory>

#include "graphics/Mesh.h"
#include "graphics/scene/IMaterial.h"
#include "graphics/scene/Transform.h"

namespace RyuRenderer::Graphics::Scene
{
    class MeshBatch
    {
    public:
        MeshBatch(std::shared_ptr<IMaterial> material);

        void Draw(const glm::mat4& view, const glm::mat4& projection) const;

        bool Match(size_t materialType) const;

        bool IsVaild() const;

        std::list<Mesh> Meshes;
        std::list<Transform> Transformers;
        std::list<std::any> MaterialDatas;
        std::shared_ptr<IMaterial> Material = nullptr;
    };
}

#endif