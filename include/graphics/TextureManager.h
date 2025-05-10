#ifndef __TEXTUREMANAGER_H__
#define __TEXTUREMANAGER_H__

#include "glad/gl.h"

#include <memory>

#include "common/Factory.h"
#include "common/Singleton.h"
#include "graphics/Texture2d.h"

namespace RyuRenderer::Graphics
{
    namespace TextureManagerImpl
    {
        class TextureManagerImpl : public Common::Factory<ITexture, TextureManagerImpl>
        {
        public:
            std::shared_ptr<Texture2d> FindOrCreate2d(const std::string& source, GLint unitIdx = 0, GLenum sWrapping = GL_REPEAT, GLenum tWrapping = GL_REPEAT);

            std::shared_ptr<Texture2d> Create2d(const std::string& source, GLint unitIdx = 0, GLenum sWrapping = GL_REPEAT, GLenum tWrapping = GL_REPEAT);

            bool BeforeCreate(const std::string& source);

            std::shared_ptr<ITexture> Find(const std::string& source) const;

            std::list<std::shared_ptr<ITexture>> FindAll(const std::string& source) const;

            bool Remove(const std::string& source);

            size_t RemoveAll(const std::string& source);
        private:
            static bool CompareTextureBySource(
                const std::shared_ptr<ITexture>& p,
                const std::string& source
            );

            using base = Common::Factory<ITexture, TextureManagerImpl>;
        };
    }

    class TextureManager : public Common::Singleton<TextureManagerImpl::TextureManagerImpl> {};
}

#endif