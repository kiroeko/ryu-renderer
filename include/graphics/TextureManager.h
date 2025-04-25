#ifndef __TEXTUREMANAGER_H__
#define __TEXTUREMANAGER_H__

#include "glad/gl.h"

#include <functional>
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
            std::shared_ptr<ITexture> FindOrCreate2d(const std::string& source, GLint unitIdx = 0)
            {
                auto p = Find(source);
                if (p)
                    return p;

                return Create2d(source, unitIdx);
            }

            std::shared_ptr<Texture2d> Create2d(const std::string& source, GLint unitIdx = 0)
            {
                std::unique_lock<std::shared_mutex> lock(mutex);
                std::shared_ptr<Texture2d> p = std::make_shared<Texture2d>(source, unitIdx);
                if (p)
                {
                    products.emplace_back(p);
                    lock.unlock();
                    AfterCreate(p);
                }

                return p;
            }

            bool BeforeCreate(const std::string& source)
            {
                auto p = Find(source);
                if (p)
                    return false;
                return true;
            }

            std::shared_ptr<ITexture> Find(const std::string& source) const
            {
                auto predicate = std::bind(
                    &CompareTextureBySource,
                    std::placeholders::_1,
                    source
                );

                return base::Find(predicate);
            }

            std::list<std::shared_ptr<ITexture>> FindAll(const std::string& source) const
            {
                auto predicate = std::bind(
                    &CompareTextureBySource,
                    std::placeholders::_1,
                    source
                );

                return base::FindAll(predicate);
            }

            bool Remove(const std::string& source)
            {
                auto predicate = std::bind(
                    &CompareTextureBySource,
                    std::placeholders::_1,
                    source
                );

                return base::Remove(predicate);
            }

            size_t RemoveAll(const std::string& source)
            {
                auto predicate = std::bind(
                    &CompareTextureBySource,
                    std::placeholders::_1,
                    source
                );

                return base::RemoveAll(predicate);
            }
        private:
            static bool CompareTextureBySource(
                const std::shared_ptr<ITexture>& p,
                const std::string& source
            )
            {
                if (!p)
                    return false;
                return p->GetSource() == source;
            }

            using base = Common::Factory<ITexture, TextureManagerImpl>;
        };
    }

    class TextureManager : public Common::Singleton<TextureManagerImpl::TextureManagerImpl> {};
}

#endif