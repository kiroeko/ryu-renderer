#ifndef __SHADER_H__
#define __SHADER_H__

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/FileUtils.h"

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>

namespace OGLRenderer::Graphics
{
    class Shader
    {
    public:
        Shader() = default;

        Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
        {
            GLuint vs = 0;
            if (!CreateAndComplieShader(vertexShaderFilePath, GL_VERTEX_SHADER, vs))
                return;

            GLuint fs = 0;
            if (!CreateAndComplieShader(fragmentShaderFilePath, GL_FRAGMENT_SHADER, fs))
                return;

            shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vs);
            glAttachShader(shaderProgram, fs);
            glLinkProgram(shaderProgram);
            int success = 0;
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            if (!success)
            {
                constexpr int errlogLen = 4096;
                char errLog[errlogLen];
                glGetProgramInfoLog(shaderProgram, 4096, NULL, errLog);
                std::cerr <<
                    "ERROR: shader program (vs: \"" <<vertexShaderFilePath <<
                    "\",fs: \"" << fragmentShaderFilePath << "\") linking failed!\n" <<
                    errLog << std::endl;
            }

            glDeleteShader(vs);
            glDeleteShader(fs);
        }

        Shader(const Shader& other) = delete;

        Shader(Shader&& other) noexcept
            : shaderProgram(other.shaderProgram), uniformLocations(other.uniformLocations)
        {
            other.shaderProgram = 0;
            other.uniformLocations.clear();
        }

        ~Shader()
        {
            if (!IsValid())
                return;

            glDeleteProgram(shaderProgram);
            shaderProgram = 0;
            uniformLocations.clear();
        }

        Shader& operator=(Shader& other) = delete;

        Shader& operator=(Shader&& other) noexcept
        {
            if (this == &other)
                return *this;

            shaderProgram = other.shaderProgram;
            uniformLocations = other.uniformLocations;
            other.shaderProgram = 0;
            other.uniformLocations.clear();
            return *this;
        }

        bool Use()
        {
            if (!IsValid())
                return false;

            glUseProgram(shaderProgram);
            return true;
        }

        bool SetUniformWithMatrixFloat(const std::string& uniformName, const glm::mat4& mat)
        {
            if (!IsValid())
                return false;

            GLint loc = GetUniformLocation(uniformName);
            if (loc == -1)
                return false;

            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
            return true;
        }

        bool SetUniformWithFloat(const std::string& uniformName, const std::vector<float>& floats)
        {
            if (!IsValid())
                return false;

            GLint loc = GetUniformLocation(uniformName);
            if (loc == -1)
                return false;

            const auto& size = floats.size();
            if (size == 1)
            {
                glUniform1f(loc, floats[0]);
                return true;
            }
            if (size == 2)
            {
                glUniform2f(loc, floats[0], floats[1]);
                return true;
            }
            if (size == 3)
            {
                glUniform3f(loc, floats[0], floats[1], floats[2]);
                return true;
            }
            if (size == 4)
            {
                glUniform4f(loc, floats[0], floats[1], floats[2], floats[3]);
                return true;
            }
            return false;
        }

        template<typename... Args>
        std::enable_if_t<(std::is_same_v<Args, float> && ...), bool>
            SetUniformWithFloat(const std::string& uniformName, Args... args)
        {
            if (!IsValid())
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
    private:
        bool CreateAndComplieShader(
            const std::string& shaderFilePath,
            GLenum shaderType,
            GLuint& outShader)
        {
            outShader = 0;

            const auto& str =
                OGLRenderer::Common::FileUtils::GetInstance().ReadFile(shaderFilePath);
            if (str.empty())
                return false;
            const char* strC = str.c_str();

            outShader = glCreateShader(shaderType);
            glShaderSource(outShader, 1, &strC, NULL);
            glCompileShader(outShader);

            int success = 0;
            glGetShaderiv(outShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                constexpr int errlogLen = 4096;
                char errLog[errlogLen];
                glGetShaderInfoLog(outShader, errlogLen, NULL, errLog);
                std::cerr << "ERROR: shader file \"" << shaderFilePath << "\" compilation failed!\n" << errLog << std::endl;
                return false;
            };
            return true;
        }

        bool IsValid()
        {
            return shaderProgram != 0;
        }

        GLint GetUniformLocation(const std::string& uniformName)
        {
            if (!IsValid())
                return -1;

            const auto& it = uniformLocations.find(uniformName);
            if (it != uniformLocations.end())
                return it->second;

            GLint loc = glGetUniformLocation(shaderProgram, uniformName.c_str());
            if (loc == -1)
                std::cerr << "Shader uniform: \"" << uniformName << "\" not found." << std::endl;
            else
                uniformLocations[uniformName] = loc;

            return loc;
        }

        GLuint shaderProgram = 0;
        std::unordered_map<std::string, GLint> uniformLocations;
    };
}

#endif