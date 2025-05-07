#ifndef __MESHOBJECTBATCH_H__
#define __MESHOBJECTBATCH_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <list>
#include <memory>

#include "graphics/scene/MeshObject.h"
#include "graphics/scene/IMaterial.h"

namespace RyuRenderer::Graphics::Scene
{
    class MeshObjectBatch
    {
    public:
        MeshObjectBatch(std::shared_ptr<IMaterial> material);

        void Draw(const glm::mat4& view, const glm::mat4& projection) const;

        bool Match(const type_info& materialType) const;

        bool IsVaild() const;

        std::list<MeshObject> MeshObjects;
        std::shared_ptr<IMaterial> Material = nullptr;
    };
}

#endif