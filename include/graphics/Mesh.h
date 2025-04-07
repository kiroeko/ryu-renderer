#ifndef __MESH_H__
#define __MESH_H__

#define RYU_RENDERER_MACROS

#include "glad/gl.h"

#include <array>
#include <bit>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <type_traits>
#include <utility>

#include "common/Macros.h"

namespace RyuRenderer::Graphics
{
    namespace MeshImpl
    {
        template <typename T>
        struct IsStdVectorOfStdArraysImpl : std::false_type {};

        template <typename T, std::size_t N>
        requires (N > 0)
        struct IsStdVectorOfStdArraysImpl<std::vector<std::array<T, N>>> : std::true_type {};

        template <typename T>
        concept IsStdVectorOfStdArrays = IsStdVectorOfStdArraysImpl<std::remove_cvref_t<T>>::value;
    }

    class Mesh
    {
        struct VertexAttribute
        {
            VertexAttribute(GLenum dataType, unsigned long long dataStartBytesOffset, unsigned long long dataAmount)
            {
                DataType = dataType;
                DataStartBytesOffset = dataStartBytesOffset;
                DataAmount = dataAmount;
            }

            GLenum DataType = GL_NONE;
            unsigned long long DataStartBytesOffset = 0;
            unsigned long long DataAmount = 0;
        };
    public:
        Mesh() = default;

        template<typename... Args>
        requires (MeshImpl::IsStdVectorOfStdArrays<Args> && ...)
        Mesh(std::vector<GLuint> indexData, Args&&... vertexDataArgs)
        {
            if constexpr (sizeof...(Args) <= 0)
                return;
            const std::size_t vectorSize = (std::forward<Args>(vertexDataArgs).size(), ...);
            bool allSameSize = ((std::forward<Args>(vertexDataArgs).size() == vectorSize) && ...);
            FAILTEST_RTN(allSameSize, "All vertex data in vector must have the same size.")

            std::vector<std::byte> vertexData;
            unsigned long long lastDataStartBytesOffset = 0;
            std::vector<VertexAttribute> attributes;

            bool isParseFailed = false;
            for (std::size_t i = 0; i < vectorSize; ++i)
            {
                ([&] {
                    auto& vec = vertexDataArgs;
                    const auto& arr = vec[i];
                    for (std::size_t j = 0; j < arr.size(); ++j)
                    {
                        const auto& element = arr[j];
                        AppendToVertexData(vertexData, element);

                        // Vertex Attribute Collection
                        if (i == 0 && j == 0)
                        {
                            GLenum e = GL_NONE;
                            if constexpr (std::is_same_v<std::remove_cvref_t<decltype(element)>, float>)
                            {
                                e = GL_FLOAT;
                            }
                            else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(element)>, double>)
                            {
                                e = GL_DOUBLE;
                            }
                            else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(element)>, int>)
                            {
                                e = GL_INT;
                            }
                            else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(element)>, unsigned int>)
                            {
                                e = GL_UNSIGNED_INT;
                            }
                            else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(element)>, char>)
                            {
                                e = GL_BYTE;
                            }
                            else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(element)>, unsigned char>)
                            {
                                e = GL_UNSIGNED_BYTE;
                            }
                            else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(element)>, std::byte>)
                            {
                                e = GL_UNSIGNED_BYTE;
                            }
                            else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(element)>, short>)
                            {
                                e = GL_SHORT;
                            }
                            else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(element)>, unsigned short>)
                            {
                                e = GL_UNSIGNED_SHORT;
                            }
                            else
                            {
                                std::cerr << "Unsupported element type." << std::endl;
                                isParseFailed = true;
                                break;
                            }
                            auto dataAmount = arr.size();
                            attributes.emplace_back(VertexAttribute(e, lastDataStartBytesOffset, dataAmount));

                            auto arrBytes = sizeof(element) * dataAmount;
                            lastDataStartBytesOffset += arrBytes;
                        }
                    }

                    if (isParseFailed)
                        return;
                }(), ...);

                if (isParseFailed)
                    break;
            }
            if (isParseFailed)
                return;

            FAILTEST_RTN(attributes.size() < GetMaxAttributeAmount(), "Vertex attribute is oversize for OpenGL.")

            // VBOs
            glGenBuffers(1, &VBO);
            // VAOs
            glGenVertexArrays(1, &VAO);
            // IBOs
            glGenBuffers(1, &EBO);

            // Fill VBO
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(std::byte) * vertexData.size(), vertexData.data(), GL_STATIC_DRAW);

            // VAO Binding
            glBindVertexArray(VAO);
            lastestUsedVAOId = VAO;
            
            for (int i = 0; i < attributes.size(); ++i)
            {
                const auto& a = attributes[i];

                glVertexAttribPointer(i, a.DataAmount, a.DataType, GL_FALSE, lastDataStartBytesOffset, (void*)a.DataStartBytesOffset);
                glEnableVertexAttribArray(i);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            elementSize = indexData.size();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elementSize, indexData.data(), GL_STATIC_DRAW);

            // Unbind
            glBindVertexArray(0);
            lastestUsedVAOId = 0;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        Mesh(const Mesh& other) = delete;

        Mesh(Mesh&& other) noexcept :
            VAO(other.VAO),
            elementSize(other.elementSize),
            VBO(other.VBO),
            EBO(other.EBO)
        {
            other.VAO = 0;
            other.elementSize = 0;
            other.VBO = 0;
            other.EBO = 0;
        }

        ~Mesh()
        {
            elementSize = 0;

            if (VAO != 0)
            {
                if (IsUsing())
                {
                    glBindVertexArray(0);
                    lastestUsedVAOId = 0;
                }

                glDeleteVertexArrays(1, &VAO);
                VAO = 0;
            }

            if (VBO != 0)
            {
                GLint currentVBO;
                glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentVBO);
                if (currentVBO == VBO)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                }

                glDeleteBuffers(1, &VBO);
                VBO = 0;
            }

            if (EBO != 0)
            {
                GLint currentEBO;
                glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentEBO);
                if (currentEBO == EBO)
                {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                }

                glDeleteBuffers(1, &EBO);
                EBO = 0;
            }
        }

        Mesh& operator=(Mesh& other) = delete;

        Mesh& operator=(Mesh&& other) noexcept
        {
            if (this == &other)
                return *this;

            VAO = other.VAO;
            elementSize = other.elementSize;
            VBO = other.VBO;
            EBO = other.EBO;
            other.VAO = 0;
            other.elementSize = 0;
            other.VBO = 0;
            other.EBO = 0;
            return *this;
        }

        bool IsValid()
        {
            return VAO != 0 &&
                   elementSize != 0 &&
                   VBO != 0 &&
                   EBO != 0;
        }

        bool IsUsing()
        {
            if (VAO == 0)
                return false;

            if (IsCleanMode)
            {
                if (lastestUsedVAOId == 0)
                    return false;

                return lastestUsedVAOId == VAO;
            }

            GLint currentVAO;
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
            return currentVAO == VAO;
        }

        void Draw()
        {
            if (!IsValid())
                return;

            if (!IsUsing())
            {
                glBindVertexArray(VAO);
                lastestUsedVAOId = VAO;
            }

            glDrawElements(GL_TRIANGLES, elementSize, GL_UNSIGNED_INT, 0);
        }

        inline static bool IsCleanMode = true;
    private:
        template <typename T>
        requires std::is_trivially_copyable_v<T>
        static void AppendToVertexData(std::vector<std::byte> &vertexData, const T& value)
        {
            const std::byte* valueAsBytes = reinterpret_cast<const std::byte*>(&value);

            if constexpr (std::endian::native == std::endian::little)
            {
                for (int i = sizeof(T) - 1; i >= 0; --i)
                {
                    vertexData.push_back(valueAsBytes[i]);
                }
            }
            else
            {
                for (int i = 0; i < sizeof(T); ++i)
                {
                    vertexData.push_back(valueAsBytes[i]);
                }
            }
        }
    private:
        static GLint GetMaxAttributeAmount()
        {
            if (maxAttributeAmount < 0)
            {
                glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributeAmount);
            }

            return maxAttributeAmount;
        }

        GLuint VAO = 0;
        size_t elementSize = 0;
        GLuint VBO = 0;
        GLuint EBO = 0;

        inline static GLint maxAttributeAmount = -1;
        inline static GLuint lastestUsedVAOId = 0;
    };
}

#undef RYU_RENDERER_MACROS

#endif