#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "glad/gl.h"

#include <string>
#include <unordered_map>

#include "graphics/ITexture.h"

namespace RyuRenderer::Graphics
{
    class Texture2d : public ITexture
    {
    public:
        Texture2d() = default;

        Texture2d(GLenum f, GLint uIdx, int w, int h);
        
        Texture2d(const std::string& textureFilePath, GLint unitIdx = 0);

        Texture2d(const Texture2d& other) = delete;

        Texture2d(Texture2d&& other) noexcept;

        ~Texture2d();

        Texture2d& operator=(Texture2d& other) = delete;

        Texture2d& operator=(Texture2d&& other) noexcept;

        bool Use() const override;

        bool IsValid() const override;

        bool IsUsing() const override;

        GLuint GetId() const override;

        const std::string GetSource() const override;

        inline static bool IsCleanMode = true;
    private:
        void Clear();

        static GLint GetMaxTextureAmount();

        GLuint id = 0;
        GLint unitId = 0;
        GLenum format = GL_NONE;
        int width = 0;
        int height = 0;
        std::string source;

        inline static GLint maxTextureAmount = -1;
        inline static std::unordered_map<GLint, GLuint> lastestUsedTexture2dIds;
    };
}

#endif