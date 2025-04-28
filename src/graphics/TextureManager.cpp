#include "graphics/TextureManager.h"

#include <functional>

namespace RyuRenderer::Graphics::TextureManagerImpl
{
    std::shared_ptr<Texture2d> TextureManagerImpl::FindOrCreate2d(const std::string& source, GLint unitIdx)
    {
        auto p = Find(source);
        if (p)
            return std::dynamic_pointer_cast<Texture2d>(p);
        return Create2d(source, unitIdx);
    }

    std::shared_ptr<Texture2d> TextureManagerImpl::Create2d(const std::string& source, GLint unitIdx)
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

    bool TextureManagerImpl::BeforeCreate(const std::string& source)
    {
        auto p = Find(source);
        if (p)
            return false;
        return true;
    }

    std::shared_ptr<ITexture> TextureManagerImpl::Find(const std::string& source) const
    {
        auto predicate = std::bind(
            &CompareTextureBySource,
            std::placeholders::_1,
            source
        );

        return base::Find(predicate);
    }

    std::list<std::shared_ptr<ITexture>> TextureManagerImpl::FindAll(const std::string& source) const
    {
        auto predicate = std::bind(
            &CompareTextureBySource,
            std::placeholders::_1,
            source
        );

        return base::FindAll(predicate);
    }

    bool TextureManagerImpl::Remove(const std::string& source)
    {
        auto predicate = std::bind(
            &CompareTextureBySource,
            std::placeholders::_1,
            source
        );

        return base::Remove(predicate);
    }

    size_t TextureManagerImpl::RemoveAll(const std::string& source)
    {
        auto predicate = std::bind(
            &CompareTextureBySource,
            std::placeholders::_1,
            source
        );

        return base::RemoveAll(predicate);
    }

    bool TextureManagerImpl::CompareTextureBySource(
        const std::shared_ptr<ITexture>& p,
        const std::string& source
    )
    {
        if (!p)
            return false;
        return p->GetSource() == source;
    }
}