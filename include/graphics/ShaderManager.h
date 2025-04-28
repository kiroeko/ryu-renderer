#ifndef __SHADERMANAGER_H__
#define __SHADERMANAGER_H__

#include "common/Factory.h"
#include "common/Singleton.h"
#include "graphics/Shader.h"

namespace RyuRenderer::Graphics
{
    namespace ShaderManagerImpl
    {
        class ShaderManagerImpl : public Common::Factory<Shader, ShaderManagerImpl>
        {
        public:
            std::shared_ptr<Shader> FindOrCreate(
                const std::string& vertexShaderFilePath,
                const std::string& fragmentShaderFilePath
            );

            std::shared_ptr<Shader> FindOrCreate(
                const std::string& localGPUBinaryFilePath
            );

            bool BeforeCreate(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

            bool BeforeCreate(const std::string& localGPUBinaryFilePath);

            std::shared_ptr<Shader> Find(
                const std::string& vertexShaderFilePath,
                const std::string& fragmentShaderFilePath
            ) const;

            std::shared_ptr<Shader> Find(
                const std::string& localGPUBinaryFilePath
            ) const;

            std::list<std::shared_ptr<Shader>> FindAll(
                const std::string& vertexShaderFilePath,
                const std::string& fragmentShaderFilePath
            ) const;

            std::list<std::shared_ptr<Shader>> FindAll(
                const std::string& localGPUBinaryFilePath
            ) const;

            bool Remove(
                const std::string& vertexShaderFilePath,
                const std::string& fragmentShaderFilePath
            );

            bool Remove(
                const std::string& localGPUBinaryFilePath
            );

            size_t RemoveAll(
                const std::string& vertexShaderFilePath,
                const std::string& fragmentShaderFilePath
            );

            size_t RemoveAll(
                const std::string& localGPUBinaryFilePath
            );
        private:
            static bool CompareShaderBySource(
                const std::shared_ptr<Shader>& p,
                const std::string& vertexShaderFilePath,
                const std::string& fragmentShaderFilePath
            );

            static bool CompareShaderByBinarySource(
                const std::shared_ptr<Shader>& p,
                const std::string& localGPUBinaryFilePath
            );

            using base = Common::Factory<Shader, ShaderManagerImpl>;
        };
    }

    class ShaderManager : public Common::Singleton<ShaderManagerImpl::ShaderManagerImpl>{};
}

#endif