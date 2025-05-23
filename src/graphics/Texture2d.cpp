#include "graphics/Texture2d.h"

#include "stb/stb_image.h"

#include <iostream>

#include "common/Macros.h"

namespace RyuRenderer::Graphics
{
    Texture2d::Texture2d(GLenum f, GLint uIdx, int w, int h)
    {
        FAILTEST_RTN(
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
        lastestUsedTexture2dIds[unitId] = id;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        lastestUsedTexture2dIds[unitId] = 0;
    }

    Texture2d::Texture2d(const std::string& textureFilePath, GLint unitIdx, GLenum sWrapping, GLenum tWrapping)
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
        lastestUsedTexture2dIds[unitId] = id;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrapping);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrapping);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        source = textureFilePath;
        stbi_image_free(textureData);
        glBindTexture(GL_TEXTURE_2D, 0);
        lastestUsedTexture2dIds[unitId] = 0;
    }

    Texture2d::Texture2d(Texture2d&& other) noexcept
    {
        Clear();
        id = other.id;
        unitId = other.unitId;
        format = other.format;
        width = other.width;
        height = other.height;
        source = other.source;
        other.id = 0;
        other.unitId = 0;
        other.format = 0;
        other.width = 0;
        other.height = 0;
        other.source.clear();
    }

    Texture2d::~Texture2d()
    {
        Clear();
    }

    Texture2d& Texture2d::operator=(Texture2d&& other) noexcept
    {
        if (this == &other)
            return *this;

        Clear();
        id = other.id;
        unitId = other.unitId;
        format = other.format;
        width = other.width;
        height = other.height;
        source = other.source;
        other.id = 0;
        other.unitId = 0;
        other.format = GL_NONE;
        other.width = 0;
        other.height = 0;
        other.source.clear();
        return *this;
    }

    bool Texture2d::Use() const
    {
        if (!IsValid())
            return false;

        if (IsUsing())
            return true;

        glActiveTexture(unitId);
        glBindTexture(GL_TEXTURE_2D, id);
        lastestUsedTexture2dIds[unitId] = id;
        return true;
    }

    bool Texture2d::IsValid() const
    {
        return id != 0 &&
            format != GL_NONE &&
            GetTextureUnitIdx(unitId) >= 0 &&
            GetTextureUnitIdx(unitId) < GetMaxTextureAmount() &&
            width >= 0 &&
            height >= 0;
    }

    bool Texture2d::IsUsing() const
    {
        if (unitId == 0)
            return false;

        if (IsCleanMode)
        {
            const auto& it = lastestUsedTexture2dIds.find(unitId);
            if (it == lastestUsedTexture2dIds.end())
                return false;

            if (it->second == 0)
                return false;

            if (it->second == unitId)
                return true;
            return false;
        }

        GLint prevActiveUnitId;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &prevActiveUnitId);

        if (prevActiveUnitId != unitId)
            glActiveTexture(unitId);

        GLint currentId = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentId);

        bool isUsing = currentId == id;

        if (prevActiveUnitId != unitId)
            glActiveTexture(prevActiveUnitId);

        return isUsing;
    }

    GLuint Texture2d::GetId() const
    {
        return id;
    }

    std::string Texture2d::GetSource() const
    {
        if (source.empty())
        {
            std::string noSourceTextureStr =
                "F:" + std::to_string(format) +
                "/W:" + std::to_string(width) +
                "/H:" + std::to_string(height);
            return noSourceTextureStr;
        }
        return source;
    }

    void Texture2d::Clear()
    {
        if (IsUsing())
        {
            glActiveTexture(unitId);
            glBindTexture(GL_TEXTURE_2D, 0);
            lastestUsedTexture2dIds[unitId] = 0;
        }

        if (id != 0)
        {
            glDeleteTextures(1, &id);
            id = 0;
        }

        unitId = 0;
        format = GL_NONE;
        width = 0;
        height = 0;
        source.clear();
    }

    GLint Texture2d::GetMaxTextureAmount()
    {
        if (maxTextureAmount < 0)
        {
            glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureAmount);
        }

        return maxTextureAmount;
    }
}