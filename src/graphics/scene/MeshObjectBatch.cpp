#include "glad/gl.h"
#include "graphics/scene/MeshObjectBatch.h"
#include "graphics/scene/IMaterial.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <iostream>
#include <filesystem>
#include <typeinfo>

#include "graphics/ShaderManager.h"
#include "graphics/scene/PhongBlinnMaterial.h"

namespace RyuRenderer::Graphics::Scene
{
    MeshObjectBatch::MeshObjectBatch(std::shared_ptr<IMaterial> material)
    {
        Material = material;
    }

    void MeshObjectBatch::Draw(const glm::mat4& view, const glm::mat4& projection) const
    {
        if (!IsVaild())
            return;

        for (auto& mo : MeshObjects)
        {
            const auto model = mo.Transformer.GetMatrix();

            auto md = mo.MaterialData;
            if (md.type() == typeid(PhongBlinnMaterialData))
            {
                auto data = std::any_cast<PhongBlinnMaterialData>(md);
                data.Model = model;
                data.View = view;
                data.Projection = projection;
                md = data;
            }

            Material->SetData(md);
            Material->Use();

            for (const auto& m : mo.Meshes)
                m.Draw();
        }
    }
    
    bool MeshObjectBatch::Match(const type_info& materialType) const
    {
        if (typeid(*Material.get()) == materialType)
            return true;
        return false;
    }

    bool MeshObjectBatch::IsVaild() const
    {
        if (MeshObjects.empty())
            return false;
        if (!Material)
            return false;
        if (!Material->IsVaild())
            return false;
        return true;
    }
}