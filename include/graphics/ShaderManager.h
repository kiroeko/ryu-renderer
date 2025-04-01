#ifndef __SHADERMANAGER_H__
#define __SHADERMANAGER_H__

#include <functional>

#include "graphics/Shader.h"
#include "common/Factory.h"

class ShaderManager : public OGLRenderer::Common::Factory<OGLRenderer::Graphics::Shader, ShaderManager>
{
public:
    std::shared_ptr<OGLRenderer::Graphics::Shader> FindOrCreate(
        const std::string& vertexShaderFilePath,
        const std::string& fragmentShaderFilePath
    )
    {
        auto p = Find(vertexShaderFilePath, fragmentShaderFilePath);
        if (p)
            return p;

        return Create(vertexShaderFilePath, fragmentShaderFilePath);
    }

    bool BeforeCreate(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
    {
        auto p = Find(vertexShaderFilePath, fragmentShaderFilePath);
        if (p)
            return false;
        return true;
    }

    std::shared_ptr<OGLRenderer::Graphics::Shader> Find(
        const std::string& vertexShaderFilePath,
        const std::string& fragmentShaderFilePath
    )
    {
        auto predicate = std::bind(
            &CompareShader,
            std::placeholders::_1,
            vertexShaderFilePath,
            fragmentShaderFilePath
        );

        return base::Find(predicate);
    }

    std::list<std::shared_ptr<OGLRenderer::Graphics::Shader>> FindAll(
        const std::string& vertexShaderFilePath,
        const std::string& fragmentShaderFilePath
    )
    {
        auto predicate = std::bind(
            &CompareShader,
            std::placeholders::_1,
            vertexShaderFilePath,
            fragmentShaderFilePath
        );

        return base::FindAll(predicate);
    }

    bool Remove(
        const std::string& vertexShaderFilePath,
        const std::string& fragmentShaderFilePath
    )
    {
        auto predicate = std::bind(
            &CompareShader,
            std::placeholders::_1,
            vertexShaderFilePath,
            fragmentShaderFilePath
        );

        return base::Remove(predicate);
    }

    size_t RemoveAll(
        const std::string& vertexShaderFilePath,
        const std::string& fragmentShaderFilePath
    )
    {
        auto predicate = std::bind(
            &CompareShader,
            std::placeholders::_1,
            vertexShaderFilePath,
            fragmentShaderFilePath
        );

        return base::RemoveAll(predicate);
    }
private:
    static bool CompareShader(
        const std::shared_ptr<OGLRenderer::Graphics::Shader>& p,
        const std::string& vertexShaderFilePath,
        const std::string& fragmentShaderFilePath
    ) {
        if (!p)
            return false;
        return p->GetVertexShaderSource() == vertexShaderFilePath &&
               p->GetFragmentShaderSource() == fragmentShaderFilePath;
    }

using base = OGLRenderer::Common::Factory<OGLRenderer::Graphics::Shader, ShaderManager>;
};

#endif