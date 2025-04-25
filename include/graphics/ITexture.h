#ifndef __ITEXTURE_H__
#define __ITEXTURE_H__

#include <string>

#include "glad/gl.h"

namespace RyuRenderer::Graphics
{
    class ITexture
    {
    public:
        virtual bool Use() const = 0;

        virtual bool IsValid() const = 0;

        virtual bool IsUsing() const = 0;

        virtual GLuint GetId() const = 0;

        virtual const std::string GetSource() const = 0;

        static GLint GetTextureUnitId(GLint unitIdx)
        {
            return GL_TEXTURE0 + unitIdx;
        }

        static GLint GetTextureUnitIdx(GLint unitId)
        {
            return unitId - GL_TEXTURE0;
        }
    };
}

#endif