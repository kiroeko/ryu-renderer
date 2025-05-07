#include "glad/gl.h"
#include "graphics/scene/MeshBatch.h"

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
    MeshBatch::MeshBatch(std::shared_ptr<IMaterial> material)
    {
        Material = material;
    }

    void MeshBatch::Draw(const glm::mat4& view, const glm::mat4& projection) const
    {
        if (!IsVaild())
            return;

        size_t size = Meshes.size();
        for (size_t i = 0; i < size; ++i)
        {
            auto mIter = Meshes.begin();
            auto tIter = Transformers.begin();
            auto mdIter = MaterialDatas.begin();
            std::advance(mIter, i);
            std::advance(tIter, i);
            std::advance(mdIter, i);

            if (!mIter->IsValid())
                continue;

            const auto& m = *mIter;
            const auto& t = *tIter;
            std::any md = *mdIter;

            if (md.type() == typeid(PhongBlinnMaterialData))
            {
                auto data = std::any_cast<PhongBlinnMaterialData>(md);
                data.Model = t.GetMatrix();
                data.View = view;
                data.Projection = projection;
                md = data;
            }

            Material->SetData(md);
            Material->Use();
            m.Draw();
        }
    }
    
    bool MeshBatch::Match(size_t materialType) const
    {
        if (typeid(*Material.get()).hash_code() == materialType)
            return true;
        return false;
    }

    bool MeshBatch::IsVaild() const
    {
        if (Meshes.size() != Transformers.size() || Transformers.size() != MaterialDatas.size())
            return false;
        if (Meshes.empty())
            return false;
        if (!Material)
            return false;
        if (!Material->IsVaild())
            return false;
        return true;
    }
}