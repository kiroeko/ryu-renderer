#ifndef __SHADER_H__
#define __SHADER_H__

#include "glad/gl.h"
#include "glm/glm.hpp"

#include <cstddef>
#include <string>
#include <vector>
#include <unordered_map>

namespace RyuRenderer::Graphics
{
    class Shader
    {
    public:
        Shader() = default;

        Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

        Shader(const std::string& localGPUBinaryFilePath);

        Shader(const Shader& other) = delete;

        Shader(Shader&& other) noexcept;

        ~Shader();

        Shader& operator=(Shader& other) = delete;

        Shader& operator=(Shader&& other) noexcept;

        bool Use() const;

        bool SetUniform(const std::string& uniformName, const std::vector<bool>& bools);

        template<typename... Args>
        std::enable_if_t<(std::is_same_v<Args, bool> && ...), bool>
            SetUniform(const std::string& uniformName, Args... args)
        {
            if (!IsUsing())
                return false;

            GLint loc = GetUniformLocation(uniformName);
            if (loc == -1)
                return false;

            constexpr std::size_t numArgs = sizeof...(Args);
            static_assert(numArgs >= 1 && numArgs <= 4, "Error: Unsupported number of arguments in SetUniformWithBool.");

            if constexpr (numArgs == 1)
            {
                glUniform1i(loc, args...);
            }
            else if constexpr (numArgs == 2)
            {
                glUniform2i(loc, args...);
            }
            else if constexpr (numArgs == 3)
            {
                glUniform3i(loc, args...);
            }
            else if constexpr (numArgs == 4)
            {
                glUniform4i(loc, args...);
            }
            return true;
        }

        bool SetUniform(const std::string& uniformName, const std::vector<int>& ints);

        template<typename... Args>
        std::enable_if_t<(std::is_same_v<Args, int> && ...), bool>
            SetUniform(const std::string& uniformName, Args... args)
        {
            if (!IsUsing())
                return false;

            GLint loc = GetUniformLocation(uniformName);
            if (loc == -1)
                return false;

            constexpr std::size_t numArgs = sizeof...(Args);
            static_assert(numArgs >= 1 && numArgs <= 4, "Error: Unsupported number of arguments in SetUniformWithInt.");

            if constexpr (numArgs == 1)
            {
                glUniform1i(loc, args...);
            }
            else if constexpr (numArgs == 2)
            {
                glUniform2i(loc, args...);
            }
            else if constexpr (numArgs == 3)
            {
                glUniform3i(loc, args...);
            }
            else if constexpr (numArgs == 4)
            {
                glUniform4i(loc, args...);
            }
            return true;
        }

        bool SetUniform(const std::string& uniformName, const std::vector<unsigned int>& uints);

        template<typename... Args>
        std::enable_if_t<(std::is_same_v<Args, unsigned int> && ...), bool>
            SetUniform(const std::string& uniformName, Args... args)
        {
            if (!IsUsing())
                return false;

            GLint loc = GetUniformLocation(uniformName);
            if (loc == -1)
                return false;

            constexpr std::size_t numArgs = sizeof...(Args);
            static_assert(numArgs >= 1 && numArgs <= 4, "Error: Unsupported number of arguments in SetUniformWithUInt.");

            if constexpr (numArgs == 1)
            {
                glUniform1ui(loc, args...);
            }
            else if constexpr (numArgs == 2)
            {
                glUniform2ui(loc, args...);
            }
            else if constexpr (numArgs == 3)
            {
                glUniform3ui(loc, args...);
            }
            else if constexpr (numArgs == 4)
            {
                glUniform4ui(loc, args...);
            }
            return true;
        }

        bool SetUniform(const std::string& uniformName, const glm::vec2& floats);

        bool SetUniform(const std::string& uniformName, const glm::vec3& floats);

        bool SetUniform(const std::string& uniformName, const glm::vec4& floats);

        bool SetUniform(const std::string& uniformName, const std::vector<float>& floats);

        template<typename... Args>
        std::enable_if_t<(std::is_same_v<Args, float> && ...), bool>
            SetUniform(const std::string& uniformName, Args... args)
        {
            if (!IsUsing())
                return false;

            GLint loc = GetUniformLocation(uniformName);
            if (loc == -1)
                return false;

            constexpr std::size_t numArgs = sizeof...(Args);
            static_assert(numArgs >= 1 && numArgs <= 4, "Error: Unsupported number of arguments in SetUniformWithFloat.");

            if constexpr (numArgs == 1)
            {
                glUniform1f(loc, args...);
            }
            else if constexpr (numArgs == 2)
            {
                glUniform2f(loc, args...);
            }
            else if constexpr (numArgs == 3)
            {
                glUniform3f(loc, args...);
            }
            else if constexpr (numArgs == 4)
            {
                glUniform4f(loc, args...);
            }
            return true;
        }

        bool SetUniform(const std::string& uniformName, const std::vector<double>& doubles);

        template<typename... Args>
        std::enable_if_t<(std::is_same_v<Args, double> && ...), bool>
            SetUniform(const std::string& uniformName, Args... args)
        {
            if (!IsUsing())
                return false;

            GLint loc = GetUniformLocation(uniformName);
            if (loc == -1)
                return false;

            constexpr std::size_t numArgs = sizeof...(Args);
            static_assert(numArgs >= 1 && numArgs <= 4, "Error: Unsupported number of arguments in SetUniformWithDouble.");

            if constexpr (numArgs == 1)
            {
                glUniform1d(loc, args...);
            }
            else if constexpr (numArgs == 2)
            {
                glUniform2d(loc, args...);
            }
            else if constexpr (numArgs == 3)
            {
                glUniform3d(loc, args...);
            }
            else if constexpr (numArgs == 4)
            {
                glUniform4d(loc, args...);
            }
            return true;
        }

        bool SetUniform(const std::string& uniformName, const glm::mat2& mat);

        bool SetUniform(const std::string& uniformName, const glm::mat3& mat);

        bool SetUniform(const std::string& uniformName, const glm::mat4& mat);

        bool SaveLocalGPUBinaryToFile(const std::string& localGPUBinaryFilePath) const;

        bool IsValid() const;

        bool IsUsing() const;

        const std::string& GetVertexSource() const;
        const std::string& GetFragmentSource() const;
        const std::string& GetBinarySource() const;
        
        inline static bool IsCleanMode = true;
    private:
        void Clear();

        bool LoadShaderBySourceCodeFile(
            const std::string& shaderSourceCodeFilePath,
            GLenum shaderType,
            GLuint& outShader);

        bool LoadShaderBySpvFile(
            const std::string& spvFilePath,
            GLenum shaderType,
            GLuint& outShader
        );

        bool LoadShaderByLocalGPUBinaryFile(
            const std::string& spvFilePath,
            GLuint& shaderProgram
        );

        GLint GetUniformLocation(const std::string& uniformName);

        GLuint programId = 0;
        std::unordered_map<std::string, GLint> uniformLocations;

        std::string vertexSource;
        std::string fragmentSource;
        std::string binarySource;

        inline static GLuint lastestUsedProgramId = 0;
    };
}

#endif