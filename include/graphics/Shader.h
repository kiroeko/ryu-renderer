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
#include <cstddef>

namespace OGLRenderer::Graphics
{
    class Shader
    {
    public:
        Shader() = default;

        Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
        {
            GLuint vs = 0;
            GLuint fs = 0;

            if (vertexShaderFilePath.ends_with(".vert") &&
                fragmentShaderFilePath.ends_with(".frag"))
            {
                if (!LoadShaderBySourceCodeFile(vertexShaderFilePath, GL_VERTEX_SHADER, vs))
                    return;

                if (!LoadShaderBySourceCodeFile(fragmentShaderFilePath, GL_FRAGMENT_SHADER, fs))
                    return;
            }
            else if (vertexShaderFilePath.ends_with(".spv") &&
                     fragmentShaderFilePath.ends_with(".spv"))
            {
                if (!LoadShaderBySpvFile(vertexShaderFilePath, GL_VERTEX_SHADER, vs))
                    return;

                if (!LoadShaderBySpvFile(fragmentShaderFilePath, GL_FRAGMENT_SHADER, fs))
                    return;
            }
            else
            {
                return;
            }

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

                glDeleteProgram(shaderProgram);
                shaderProgram = 0;
            }

            glDeleteShader(vs);
            glDeleteShader(fs);
            vertexShaderSource = vs;
            fragmentShaderSource = fs;
        }

        Shader(const std::string& localGPUBinaryFilePath)
        {
            if (!LoadShaderByLocalGPUBinaryFile(localGPUBinaryFilePath, shaderProgram))
                return;

            binarySource = localGPUBinaryFilePath;
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

            vertexShaderSource.clear();
            fragmentShaderSource.clear();
            binarySource.clear();
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

        bool SetUniformWithInt(const std::string& uniformName, const std::vector<int>& ints)
        {
            if (!IsValid())
                return false;

            GLint loc = GetUniformLocation(uniformName);
            if (loc == -1)
                return false;

            const auto& size = ints.size();
            if (size == 1)
            {
                glUniform1i(loc, ints[0]);
                return true;
            }
            if (size == 2)
            {
                glUniform2i(loc, ints[0], ints[1]);
                return true;
            }
            if (size == 3)
            {
                glUniform3i(loc, ints[0], ints[1], ints[2]);
                return true;
            }
            if (size == 4)
            {
                glUniform4i(loc, ints[0], ints[1], ints[2], ints[3]);
                return true;
            }
            return false;
        }

        template<typename... Args>
        std::enable_if_t<(std::is_same_v<Args, int> && ...), bool>
            SetUniformWithInt(const std::string& uniformName, Args... args)
        {
            if (!IsValid())
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

        bool SaveBinaryToFile(const std::string& localGPUBinaryFilePath)
        {
            if (!IsValid())
                return false;

            GLint bufferSize = 0;
            glGetProgramiv(shaderProgram, GL_PROGRAM_BINARY_LENGTH, &bufferSize);
            if (bufferSize <= 0)
                return false;

            GLenum format = 0;
            std::vector<std::byte> buffer(bufferSize);
            glGetProgramBinary(shaderProgram, bufferSize, nullptr, &format, buffer.data());
            
            std::ofstream file(localGPUBinaryFilePath, std::ios::binary);
            if (!file.is_open())
            {
                return false;
            }
            file.write(reinterpret_cast<char*>(&format), sizeof(format));
            file.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
            file.close();

            return file.good();
        }

        const std::string& GetVertexShaderSource() const { return vertexShaderSource; }
        const std::string& GetFragmentShaderSource() const { return fragmentShaderSource; }
        const std::string& GetBinarySource() const { return binarySource; }
    private:
        bool LoadShaderBySourceCodeFile(
            const std::string& shaderSourceCodeFilePath,
            GLenum shaderType,
            GLuint& outShader)
        {
            outShader = 0;

            const auto& str =
                OGLRenderer::Common::FileUtils::GetInstance().ReadFileString(shaderSourceCodeFilePath);
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
                std::cerr << "ERROR: shader file \"" << shaderSourceCodeFilePath << "\" compilation failed!\n" << errLog << std::endl;

                glDeleteShader(outShader);
                return false;
            };
            return true;
        }

        bool LoadShaderBySpvFile(
            const std::string& spvFilePath,
            GLenum shaderType,
            GLuint& outShader
        )
        {
            outShader = 0;

            const auto& buffer =
                OGLRenderer::Common::FileUtils::GetInstance().ReadFileBinary(spvFilePath);
            if (buffer.empty())
                return false;

            outShader = glCreateShader(shaderType);
            glShaderBinary(1, &outShader, GL_SHADER_BINARY_FORMAT_SPIR_V, buffer.data(), buffer.size());
            glSpecializeShader(outShader, "main", 0, NULL, NULL);

            int success = 0;
            glGetShaderiv(outShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                constexpr int errlogLen = 4096;
                char errLog[errlogLen];
                glGetShaderInfoLog(outShader, errlogLen, NULL, errLog);
                std::cerr << "ERROR: shader spv file \"" << spvFilePath << "\" compilation failed!\n" << errLog << std::endl;

                glDeleteShader(outShader);
                return false;
            };
            return true;
        }

        bool LoadShaderByLocalGPUBinaryFile(
            const std::string& spvFilePath,
            GLuint& shaderProgram
        )
        {
            shaderProgram = 0;

            std::ifstream file(spvFilePath, std::ios::binary | std::ios::ate);
            if (!file.is_open())
            {
                return false;
            }

            auto fileSize = file.tellg();
            file.seekg(0, std::ios::beg);

            GLenum format = 0;
            file.read(reinterpret_cast<char*>(&format), sizeof(format));

            auto bufferSize = fileSize - std::streamoff(sizeof(format));
            std::vector<std::byte> buffer = std::vector<std::byte>(bufferSize);
            file.read(reinterpret_cast<char*>(buffer.data()), bufferSize);

            file.close();

            shaderProgram = glCreateProgram();
            glProgramBinary(shaderProgram, format, buffer.data(), buffer.size());

            int success = 0;
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            if (!success)
            {
                constexpr int errlogLen = 4096;
                char errLog[errlogLen];
                glGetProgramInfoLog(shaderProgram, 4096, NULL, errLog);
                std::cerr <<
                    "ERROR: shader program binary file: \"" << spvFilePath <<
                    "\" linking failed!\n" <<
                    errLog << std::endl;

                glDeleteProgram(shaderProgram);
                shaderProgram = 0;
                return false;
            }

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

        std::string vertexShaderSource;
        std::string fragmentShaderSource;
        std::string binarySource;
    };
}

#endif