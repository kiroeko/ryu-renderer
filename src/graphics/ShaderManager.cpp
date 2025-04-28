#include "graphics/ShaderManager.h"

#include <functional>

namespace RyuRenderer::Graphics::ShaderManagerImpl
{
    std::shared_ptr<Shader> ShaderManagerImpl::FindOrCreate(
        const std::string& vertexShaderFilePath,
        const std::string& fragmentShaderFilePath
    )
    {
        auto p = Find(vertexShaderFilePath, fragmentShaderFilePath);
        if (p)
            return p;

        return Create(vertexShaderFilePath, fragmentShaderFilePath);
    }

    std::shared_ptr<Shader> ShaderManagerImpl::FindOrCreate(
        const std::string& localGPUBinaryFilePath
    )
    {
        auto p = Find(localGPUBinaryFilePath);
        if (p)
            return p;

        return Create(localGPUBinaryFilePath);
    }

    bool ShaderManagerImpl::BeforeCreate(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
    {
        auto p = Find(vertexShaderFilePath, fragmentShaderFilePath);
        if (p)
            return false;
        return true;
    }

    bool ShaderManagerImpl::BeforeCreate(const std::string& localGPUBinaryFilePath)
    {
        auto p = Find(localGPUBinaryFilePath);
        if (p)
            return false;
        return true;
    }

    std::shared_ptr<Shader> ShaderManagerImpl::Find(
        const std::string& vertexShaderFilePath,
        const std::string& fragmentShaderFilePath
    ) const
    {
        auto predicate = std::bind(
            &CompareShaderBySource,
            std::placeholders::_1,
            vertexShaderFilePath,
            fragmentShaderFilePath
        );

        return base::Find(predicate);
    }

    std::shared_ptr<Shader> ShaderManagerImpl::Find(
        const std::string& localGPUBinaryFilePath
    ) const
    {
        auto predicate = std::bind(
            &CompareShaderByBinarySource,
            std::placeholders::_1,
            localGPUBinaryFilePath
        );

        return base::Find(predicate);
    }

    std::list<std::shared_ptr<Shader>> ShaderManagerImpl::FindAll(
        const std::string& vertexShaderFilePath,
        const std::string& fragmentShaderFilePath
    ) const
    {
        auto predicate = std::bind(
            &CompareShaderBySource,
            std::placeholders::_1,
            vertexShaderFilePath,
            fragmentShaderFilePath
        );

        return base::FindAll(predicate);
    }

    std::list<std::shared_ptr<Shader>> ShaderManagerImpl::FindAll(
        const std::string& localGPUBinaryFilePath
    ) const
    {
        auto predicate = std::bind(
            &CompareShaderByBinarySource,
            std::placeholders::_1,
            localGPUBinaryFilePath
        );

        return base::FindAll(predicate);
    }

    bool ShaderManagerImpl::Remove(
        const std::string& vertexShaderFilePath,
        const std::string& fragmentShaderFilePath
    )
    {
        auto predicate = std::bind(
            &CompareShaderBySource,
            std::placeholders::_1,
            vertexShaderFilePath,
            fragmentShaderFilePath
        );

        return base::Remove(predicate);
    }

    bool ShaderManagerImpl::Remove(
        const std::string& localGPUBinaryFilePath
    )
    {
        auto predicate = std::bind(
            &CompareShaderByBinarySource,
            std::placeholders::_1,
            localGPUBinaryFilePath
        );

        return base::Remove(predicate);
    }

    size_t ShaderManagerImpl::RemoveAll(
        const std::string& vertexShaderFilePath,
        const std::string& fragmentShaderFilePath
    )
    {
        auto predicate = std::bind(
            &CompareShaderBySource,
            std::placeholders::_1,
            vertexShaderFilePath,
            fragmentShaderFilePath
        );

        return base::RemoveAll(predicate);
    }

    size_t ShaderManagerImpl::RemoveAll(
        const std::string& localGPUBinaryFilePath
    )
    {
        auto predicate = std::bind(
            &CompareShaderByBinarySource,
            std::placeholders::_1,
            localGPUBinaryFilePath
        );

        return base::RemoveAll(predicate);
    }

    bool ShaderManagerImpl::CompareShaderBySource(
        const std::shared_ptr<Shader>& p,
        const std::string& vertexShaderFilePath,
        const std::string& fragmentShaderFilePath
    ) {
        if (!p)
            return false;
        return p->GetVertexSource() == vertexShaderFilePath &&
            p->GetFragmentSource() == fragmentShaderFilePath;
    }

    bool ShaderManagerImpl::CompareShaderByBinarySource(
        const std::shared_ptr<Shader>& p,
        const std::string& localGPUBinaryFilePath
    ) {
        if (!p)
            return false;
        return p->GetBinarySource() == localGPUBinaryFilePath;
    }
}