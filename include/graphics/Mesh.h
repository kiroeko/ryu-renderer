#ifndef __MESH_H__
#define __MESH_H__

#include "glad/gl.h"

#include <array>
#include <bit>
#include <cstddef>
#include <stdexcept>
#include <vector>
#include <type_traits>
#include <utility>

namespace RyuRenderer::Graphics
{
    namespace MeshImpl
    {
        template <typename T>
        struct IsStdArrayImpl : std::false_type {};

        template <typename T, std::size_t N>
        struct IsStdArrayImpl<std::array<T, N>> : std::true_type {};

        template <typename T>
        concept IsStdArray = IsStdArrayImpl<std::remove_cvref_t<T>>::value;

        template <typename T>
        struct IsStdVectorOfStdArraysImpl : std::false_type {};

        template <typename T, std::size_t N>
        struct IsStdVectorOfStdArraysImpl<std::vector<std::array<T, N>>> : std::true_type {};

        template <typename T>
        concept IsStdVectorOfStdArrays = IsStdVectorOfStdArraysImpl<std::remove_cvref_t<T>>::value;
    }

    class Mesh
    {
        struct VertexAttribute
        {
            VertexAttribute(GLenum dataType, unsigned long long dataStartBytesOffset, unsigned long long dataSize)
            {
                DataType = dataType;
                DataStartBytesOffset = dataStartBytesOffset;
                DataSize = dataSize;
            }

            GLenum DataType = GL_NONE;
            unsigned long long DataStartBytesOffset = 0;
            unsigned long long DataSize = 0;
        };
    public:
        template<typename... Args>
        requires (MeshImpl::IsStdVectorOfStdArrays<Args> && ...)
        Mesh(std::vector<GLuint> vertexIndices, Args&&... vertexDataArgs)
        {
            if constexpr (sizeof...(Args) <= 0)
                return;
            const std::size_t vectorSize = (std::forward<Args>(vertexDataArgs).size(), ...);
            bool allSameSize = ((std::forward<Args>(vertexDataArgs).size() == vectorSize) && ...);
            if (!allSameSize)
                throw std::invalid_argument("All vertex data in vector must have the same size.");

            unsigned long long lastDataStartBytesOffset = 0;
            std::vector<VertexAttribute> attributes;

            for (std::size_t i = 0; i < vectorSize; ++i)
            {
                ([&] {
                    auto& vec = vertexDataArgs;
                    const auto& arr = vec[i];
                    for (std::size_t j = 0; j < arr.size(); ++j)
                    {
                        const auto& element = arr[j];
                        AppendToVertexData(element);

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
                                throw std::runtime_error("Unsupported element type");
                            }
                            auto dataSize = arr.size();
                            attributes.emplace_back(VertexAttribute(e, lastDataStartBytesOffset, dataSize));

                            auto arrBytes = sizeof(element) * dataSize;
                            lastDataStartBytesOffset += arrBytes;
                        }
                    }
                }(), ...);
            }

            int ie143 = 10;
        }
    private:
        template <typename T>
        requires std::is_trivially_copyable_v<T>
        void AppendToVertexData(const T& value)
        {
            vertexData.clear();

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

        std::vector<std::byte> vertexData;
        std::vector<GLuint> indexData;

        GLuint VAO = 0;
    };
}

#endif