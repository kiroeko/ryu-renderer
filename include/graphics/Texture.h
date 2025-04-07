#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "glad/gl.h"
#include "stb/stb_image.h"

#include <iostream>
#include <string>

namespace RyuRenderer::Graphics
{
    class Texture
    {
    public:
        Texture() = default;

        Texture(GLenum t, GLenum f, GLint unitIdx, int w, int h)
        {
            if (unitIdx >= GetMaxTextureAmount())
                throw std::invalid_argument("Texture unit id is oversize for OpenGL.");
            unitId = unitIdx;

            if (t == 0)
                throw std::invalid_argument("Texture type is invalid.");
            type = t;

            if (f == 0)
                throw std::invalid_argument("Texture format is invalid.");
            format = f;

            if (w < 0)
                throw std::invalid_argument("Texture width is invalid.");
            width = w;

            if (h < 0)
                throw std::invalid_argument("Texture width is invalid.");
            height = h;

            glGenTextures(1, &id);
            glActiveTexture(GetTextureUnit(unitId));
            glTexImage2D(type, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
            glGenerateMipmap(type);

            glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindTexture(type, 0);
        }
        
        Texture(const std::string& textureFilePath, GLint unitIdx)
        {

        }

        bool Use()
        {
            if (!IsValid())
                return false;

            glActiveTexture(GetTextureUnit(unitId));
            glBindTexture(GL_TEXTURE_2D, id);
            return true;
        }

        bool IsValid()
        {
            return id != 0 &&
                   type != GL_NONE &&
                   format != GL_NONE &&
                   unitId <= 0 &&
                   unitId > maxTextureAmount;
        }

        bool IsUsing()
        {
            if (id == 0)
                return false;

            GLint currentProgram = 0;
            glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
            if (currentProgram == 0)
                return false;

            return id == currentProgram;
        }

        static GLint GetTextureUnit(GLint unitId)
        {
            return GL_TEXTURE0 + unitId;
        }
    private:
        bool CreateTexture2D(const std::string& textureFilePath, GLint unitIdx, GLenum f)
        {
            if (maxTextureAmount <= 0)
            {
                glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureAmount);
            }
            if (unitIdx >= maxTextureAmount)
                throw std::invalid_argument("Texture unit id is oversize for OpenGL.");
            unitId = unitIdx;

            if (f == 0)
                throw std::invalid_argument("Texture format is invalid.");
            format = f;

            type = GL_TEXTURE_2D;

            glGenTextures(1, &id);
            glActiveTexture(GetTextureUnit(unitId));
            glBindTexture(type, id);

            int textureNRChannels = 0;
            unsigned char* textureData = stbi_load(textureFilePath.c_str(), &width, &height, &textureNRChannels, 0);
            if (textureData)
            {
                glTexImage2D(type, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
                glGenerateMipmap(type);

                glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            else
            {
                glBindTexture(type, 0);
                glDeleteTextures(1, &id);
                id = 0;
                type = GL_NONE;
                format = GL_NONE;
                unitId = 0;
                width = 0;
                height = 0;

                std::cerr << "Failed to load texture" << std::endl;
            }
            stbi_image_free(textureData);

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        static GLint GetMaxTextureAmount()
        {
            if (maxTextureAmount < 0)
            {
                glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureAmount);
            }

            return maxTextureAmount;
        }

        GLuint id = 0;
        GLenum type = GL_NONE;
        GLenum format = GL_NONE;
        GLint unitId = 0;
        int width = 0;
        int height = 0;

        inline static GLint maxTextureAmount = -1;
    };
}

#endif