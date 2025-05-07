#ifndef __PHONGBLINNMATERIAL_H__
#define __PHONGBLINNMATERIAL_H__

#include <any>

#include "graphics/scene/PhongBlinnMaterialData.h"
#include "graphics/scene/IMaterial.h"

namespace RyuRenderer::Graphics::Scene
{
    class PhongBlinnMaterial : public IMaterial
    {
    public:
        PhongBlinnMaterial();

        void SetData(const std::any& d) override;

        void Use() const override;
    private:
        PhongBlinnMaterialData data;
    };
}

#endif