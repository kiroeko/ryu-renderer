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

            for (std::size_t i = 0; i < vectorSize; ++i)
            {
                ([&] {
                    auto& vec = vertexDataArgs;
                    const auto& arr = vec[i];
                    for (const auto& element : arr)
                    {
                        AppendToVertexData(element);
                    }
                }(), ...);
            }
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