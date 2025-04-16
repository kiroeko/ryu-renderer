#ifndef __SHADERMANAGER_H__
#define __SHADERMANAGER_H__

#include <functional>

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
            )
            {
                auto p = Find(vertexShaderFilePath, fragmentShaderFilePath);
                if (p)
                    return p;

                return Create(vertexShaderFilePath, fragmentShaderFilePath);
            }

            std::shared_ptr<Shader> FindOrCreate(
                const std::string& localGPUBinaryFilePath
            )
            {
                auto p = Find(localGPUBinaryFilePath);
                if (p)
                    return p;

                return Create(localGPUBinaryFilePath);
            }

            bool BeforeCreate(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
            {
                auto p = Find(vertexShaderFilePath, fragmentShaderFilePath);
                if (p)
                    return false;
                return true;
            }

            bool BeforeCreate(const std::string& localGPUBinaryFilePath)
            {
                auto p = Find(localGPUBinaryFilePath);
                if (p)
                    return false;
                return true;
            }

            std::shared_ptr<Shader> Find(
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

            std::shared_ptr<Shader> Find(
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

            std::list<std::shared_ptr<Shader>> FindAll(
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

            std::list<std::shared_ptr<Shader>> FindAll(
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

            bool Remove(
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

            bool Remove(
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

            size_t RemoveAll(
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

            size_t RemoveAll(
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
        private:
            static bool CompareShaderBySource(
                const std::shared_ptr<Shader>& p,
                const std::string& vertexShaderFilePath,
                const std::string& fragmentShaderFilePath
            ) {
                if (!p)
                    return false;
                return p->GetVertexSource() == vertexShaderFilePath &&
                    p->GetFragmentSource() == fragmentShaderFilePath;
            }

            static bool CompareShaderByBinarySource(
                const std::shared_ptr<Shader>& p,
                const std::string& localGPUBinaryFilePath
            ) {
                if (!p)
                    return false;
                return p->GetBinarySource() == localGPUBinaryFilePath;
            }

            using base = Common::Factory<Shader, ShaderManagerImpl>;
        };
    }

    class ShaderManager : public Common::Singleton<ShaderManagerImpl::ShaderManagerImpl>{};
}


#endif