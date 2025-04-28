#include "graphics/Shader.h"

#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include "common/FileUtils.h"

namespace RyuRenderer::Graphics
{
    Shader::Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
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

        programId = glCreateProgram();
        glAttachShader(programId, vs);
        glAttachShader(programId, fs);
        glLinkProgram(programId);

        int success = 0;
        glGetProgramiv(programId, GL_LINK_STATUS, &success);
        if (!success)
        {
            constexpr int errlogLen = 4096;
            char errLog[errlogLen];
            glGetProgramInfoLog(programId, 4096, NULL, errLog);
            std::cerr << "ERROR: shader program (vs: \"" << vertexShaderFilePath << "\",fs: \"" << fragmentShaderFilePath << "\") linking failed!\n" << errLog << std::endl;
            glDeleteProgram(programId);
            programId = 0;
        }

        glDeleteShader(vs);
        glDeleteShader(fs);
        vertexSource = vs;
        fragmentSource = fs;
    }

    Shader::Shader(const std::string& localGPUBinaryFilePath)
    {
        if (!LoadShaderByLocalGPUBinaryFile(localGPUBinaryFilePath, programId))
            return;

        binarySource = localGPUBinaryFilePath;
    }

    Shader::Shader(Shader&& other) noexcept
    {
        Clear();
        programId = other.programId;
        uniformLocations = other.uniformLocations;
        vertexSource = other.vertexSource;
        fragmentSource = other.fragmentSource;
        binarySource = other.binarySource;
        other.programId = 0;
        other.uniformLocations.clear();
        other.vertexSource.clear();
        other.fragmentSource.clear();
        other.binarySource.clear();
    }

    Shader::~Shader()
    {
        Clear();
    }

    Shader& Shader::operator=(Shader&& other) noexcept
    {
        if (this == &other)
            return *this;

        Clear();
        programId = other.programId;
        uniformLocations = other.uniformLocations;
        vertexSource = other.vertexSource;
        fragmentSource = other.fragmentSource;
        binarySource = other.binarySource;
        other.programId = 0;
        other.uniformLocations.clear();
        other.vertexSource.clear();
        other.fragmentSource.clear();
        other.binarySource.clear();
        return *this;
    }

    bool Shader::Use() const
    {
        if (!IsValid())
            return false;

        if (IsUsing())
            return true;

        glUseProgram(programId);
        lastestUsedProgramId = programId;
        return true;
    }

    bool Shader::SetUniform(const std::string& uniformName, const std::vector<bool>& bools)
    {
        if (!IsUsing())
            return false;

        GLint loc = GetUniformLocation(uniformName);
        if (loc == -1)
            return false;

        const auto& size = bools.size();
        if (size == 1)
        {
            glUniform1i(loc, bools[0]);
            return true;
        }
        if (size == 2)
        {
            glUniform2i(loc, bools[0], bools[1]);
            return true;
        }
        if (size == 3)
        {
            glUniform3i(loc, bools[0], bools[1], bools[2]);
            return true;
        }
        if (size == 4)
        {
            glUniform4i(loc, bools[0], bools[1], bools[2], bools[3]);
            return true;
        }
        return false;
    }

    bool Shader::SetUniform(const std::string& uniformName, const std::vector<int>& ints)
    {
        if (!IsUsing())
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

    bool Shader::SetUniform(const std::string& uniformName, const std::vector<unsigned int>& uints)
    {
        if (!IsUsing())
            return false;

        GLint loc = GetUniformLocation(uniformName);
        if (loc == -1)
            return false;

        const auto& size = uints.size();
        if (size == 1)
        {
            glUniform1ui(loc, uints[0]);
            return true;
        }
        if (size == 2)
        {
            glUniform2ui(loc, uints[0], uints[1]);
            return true;
        }
        if (size == 3)
        {
            glUniform3ui(loc, uints[0], uints[1], uints[2]);
            return true;
        }
        if (size == 4)
        {
            glUniform4ui(loc, uints[0], uints[1], uints[2], uints[3]);
            return true;
        }
        return false;
    }

    bool Shader::SetUniform(const std::string& uniformName, const glm::vec2& floats)
    {
        if (!IsUsing())
            return false;

        GLint loc = GetUniformLocation(uniformName);
        if (loc == -1)
            return false;

        glUniform2f(loc, floats.x, floats.y);
        return true;
    }

    bool Shader::SetUniform(const std::string& uniformName, const glm::vec3& floats)
    {
        if (!IsUsing())
            return false;

        GLint loc = GetUniformLocation(uniformName);
        if (loc == -1)
            return false;

        glUniform3f(loc, floats.x, floats.y, floats.z);
        return true;
    }

    bool Shader::SetUniform(const std::string& uniformName, const glm::vec4& floats)
    {
        if (!IsUsing())
            return false;

        GLint loc = GetUniformLocation(uniformName);
        if (loc == -1)
            return false;

        glUniform4f(loc, floats.x, floats.y, floats.z, floats.w);
        return true;
    }

    bool Shader::SetUniform(const std::string& uniformName, const std::vector<float>& floats)
    {
        if (!IsUsing())
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

    bool Shader::SetUniform(const std::string& uniformName, const std::vector<double>& doubles)
    {
        if (!IsUsing())
            return false;

        GLint loc = GetUniformLocation(uniformName);
        if (loc == -1)
            return false;

        const auto& size = doubles.size();
        if (size == 1)
        {
            glUniform1d(loc, doubles[0]);
            return true;
        }
        if (size == 2)
        {
            glUniform2d(loc, doubles[0], doubles[1]);
            return true;
        }
        if (size == 3)
        {
            glUniform3d(loc, doubles[0], doubles[1], doubles[2]);
            return true;
        }
        if (size == 4)
        {
            glUniform4d(loc, doubles[0], doubles[1], doubles[2], doubles[3]);
            return true;
        }
        return false;
    }

    bool Shader::SetUniform(const std::string& uniformName, const glm::mat2& mat)
    {
        if (!IsUsing())
            return false;

        GLint loc = GetUniformLocation(uniformName);
        if (loc == -1)
            return false;

        glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
        return true;
    }

    bool Shader::SetUniform(const std::string& uniformName, const glm::mat3& mat)
    {
        if (!IsUsing())
            return false;

        GLint loc = GetUniformLocation(uniformName);
        if (loc == -1)
            return false;

        glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
        return true;
    }

    bool Shader::SetUniform(const std::string& uniformName, const glm::mat4& mat)
    {
        if (!IsUsing())
            return false;

        GLint loc = GetUniformLocation(uniformName);
        if (loc == -1)
            return false;

        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
        return true;
    }

    bool Shader::SaveLocalGPUBinaryToFile(const std::string& localGPUBinaryFilePath) const
    {
        if (!IsValid())
            return false;

        GLint bufferSize = 0;
        glGetProgramiv(programId, GL_PROGRAM_BINARY_LENGTH, &bufferSize);
        if (bufferSize <= 0)
            return false;

        GLenum format = 0;
        std::vector<std::byte> buffer(bufferSize);
        glGetProgramBinary(programId, bufferSize, nullptr, &format, buffer.data());

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

    bool Shader::IsValid() const
    {
        return programId != 0;
    }

    bool Shader::IsUsing() const
    {
        if (programId == 0)
            return false;

        if (IsCleanMode)
        {
            if (lastestUsedProgramId == 0)
                return false;

            return lastestUsedProgramId == programId;
        }

        GLint currentProgram = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        if (currentProgram == 0)
            return false;

        return programId == currentProgram;
    }

    const std::string& Shader::GetVertexSource() const { return vertexSource; }
    const std::string& Shader::GetFragmentSource() const { return fragmentSource; }
    const std::string& Shader::GetBinarySource() const { return binarySource; }

    void Shader::Clear()
    {
        if (programId != 0)
        {
            glDeleteProgram(programId);
            programId = 0;
        }

        uniformLocations.clear();

        vertexSource.clear();
        fragmentSource.clear();
        binarySource.clear();
    }

    bool Shader::LoadShaderBySourceCodeFile(
        const std::string& shaderSourceCodeFilePath,
        GLenum shaderType,
        GLuint& outShader)
    {
        outShader = 0;

        const auto& str =
            Common::FileUtils::GetInstance().ReadFileString(shaderSourceCodeFilePath);
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

    bool Shader::LoadShaderBySpvFile(
        const std::string& spvFilePath,
        GLenum shaderType,
        GLuint& outShader
    )
    {
        outShader = 0;

        const auto& buffer =
            Common::FileUtils::GetInstance().ReadFileBinary(spvFilePath);
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

    bool Shader::LoadShaderByLocalGPUBinaryFile(
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
            std::cerr << "ERROR: shader program binary file: \"" << spvFilePath << "\" linking failed!\n" << errLog << std::endl;
            glDeleteProgram(shaderProgram);
            shaderProgram = 0;
            return false;
        }

        return true;
    }

    GLint Shader::GetUniformLocation(const std::string& uniformName)
    {
        if (!IsValid())
            return -1;

        const auto& it = uniformLocations.find(uniformName);
        if (it != uniformLocations.end())
            return it->second;

        GLint loc = glGetUniformLocation(programId, uniformName.c_str());
        if (loc == -1)
            std::cerr << "Shader uniform: \"" << uniformName << "\" not found." << std::endl;
        else
            uniformLocations[uniformName] = loc;

        return loc;
    }
}