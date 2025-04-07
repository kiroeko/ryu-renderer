#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#define RYU_RENDERER_MACROS

#include "glad/gl.h"
#include "stb/stb_image.h"

#include <iostream>
#include <string>

#include "common/Macros.h"

namespace RyuRenderer::Graphics
{
    class Texture2D
    {
    public:
        Texture2D() = default;

        Texture2D(GLenum t, GLenum f, GLint uIdx, int w, int h)
        {
            FAILTEST_RTN(
                t != GL_NONE &&
                f != GL_NONE &&
                uIdx >= 0 ||
                uIdx < GetMaxTextureAmount() &&
                w >= 0 &&
                h >= 0,
                "Invaild texture arguements");

            unitId = GetTextureUnitId(uIdx);
            format = f;
            width = w;
            height = h;

            glGenTextures(1, &id);
            glActiveTexture(unitId);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);
        }
        
        Texture2D(const std::string& textureFilePath, GLint unitIdx)
        {
            FAILTEST_RTN(unitIdx >= 0 && unitIdx < GetMaxTextureAmount(), "Texture unit id is oversize for OpenGL.");
            
            if (textureFilePath.ends_with(".jpg"))
            {
                format = GL_RGB;
            }
            else if (textureFilePath.ends_with(".png"))
            {
                format = GL_RGBA;
            }
            else
            {
                std::cerr << "The suffix of the image file should be jpg or png." << std::endl;
                return;
            }

            int textureNRChannels = 0;
            unsigned char* textureData = stbi_load(textureFilePath.c_str(), &width, &height, &textureNRChannels, 0);
            if (!textureData)
            {
                width = 0;
                height = 0;
                std::cerr << "Can not load texture file: " << textureFilePath << "." << std::endl;
                return;
            }

            unitId = GetTextureUnitId(unitIdx);

            glGenTextures(1, &id);
            glActiveTexture(unitId);
            glBindTexture(GL_TEXTURE_2D, id);

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(textureData);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        Texture2D(const Texture2D& other) = delete;

        Texture2D(Texture2D&& other) noexcept :
            id(other.id),
            unitId(other.unitId),
            format(other.format),
            width(other.width),
            height(other.height)
        {
            other.id = 0;
            other.unitId = 0;
            other.format = 0;
            other.width = 0;
            other.height = 0;
        }

        ~Texture2D()
        {
            if (IsUsing())
            {
                glActiveTexture(unitId);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            glDeleteTextures(1, &id);
            id = 0;
            unitId = 0;
            format = GL_NONE;
            width = 0;
            height = 0;
        }

        Texture2D& operator=(Texture2D& other) = delete;

        Texture2D& operator=(Texture2D&& other) noexcept
        {
            if (this == &other)
                return *this;

            id = other.id;
            unitId = other.unitId;
            format = other.format;
            width = other.width;
            height = other.height;
            other.id = 0;
            other.unitId = 0;
            other.format = GL_NONE;
            other.width = 0;
            other.height = 0;
            return *this;
        }

        bool Use()
        {
            if (!IsValid())
                return false;

            if (IsUsing())
                return true;

            glActiveTexture(unitId);
            glBindTexture(GL_TEXTURE_2D, id);
            return true;
        }

        bool IsUsing()
        {
            GLint prevActiveUnitId;
            glGetIntegerv(GL_ACTIVE_TEXTURE, &prevActiveUnitId);

            if(prevActiveUnitId != unitId)
                glActiveTexture(unitId);

            GLint currentId = 0;
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentId);

            bool isUsing = currentId == id;

            if (prevActiveUnitId != unitId)
                glActiveTexture(prevActiveUnitId);

            return isUsing;
        }

        bool IsValid()
        {
            return id != 0 &&
                format != GL_NONE &&
                GetTextureUnitIdx(unitId) >= 0 &&
                GetTextureUnitIdx(unitId) < GetMaxTextureAmount() &&
                width >= 0 &&
                height >= 0;
        }

        static GLint GetTextureUnitId(GLint unitIdx)
        {
            return GL_TEXTURE0 + unitIdx;
        }

        static GLint GetTextureUnitIdx(GLint unitId)
        {
            return unitId - GL_TEXTURE0;
        }
    private:
        static GLint GetMaxTextureAmount()
        {
            if (maxTextureAmount < 0)
            {
                glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureAmount);
            }

            return maxTextureAmount;
        }

        GLuint id = 0;
        GLint unitId = 0;
        GLenum format = GL_NONE;
        int width = 0;
        int height = 0;

        inline static GLint maxTextureAmount = -1;
    };
}

#undef RYU_RENDERER_MACROS

#endif