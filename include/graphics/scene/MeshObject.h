#ifndef __MESHOBJECT_H__
#define __MESHOBJECT_H__

#include <any>
#include <list>

#include "graphics/Mesh.h"
#include "graphics/scene/Transform.h"

namespace RyuRenderer::Graphics::Scene
{
    struct MeshObject
    {
        std::list<Mesh> Meshes;
        Transform Transformer;
        std::any MaterialData;
    };
}

#endif