#ifndef __FILEUTILS_H__
#define __FILEUTILS_H__

#include "common/Singleton.h"

#include <fstream>
#include <filesystem>
#include <string>
#include <sstream>
#include <vector>
#include <cstddef>

namespace RyuRenderer::Common
{
    class FileUtils : public Singleton<FileUtils>
    {
    public:
        std::string ReadFileString(std::string filePath)
        {
            std::ifstream fileStream(filePath);
            if (!fileStream.is_open())
                return std::string();
            std::stringstream fileStrStream;
            fileStrStream << fileStream.rdbuf();
            return fileStrStream.str();
        }

        std::vector<std::byte> ReadFileBinary(std::string filePath)
        {
            std::ifstream file(filePath, std::ios::binary | std::ios::ate);
            if (!file.is_open())
            {
                return std::vector<std::byte>();
            }

            size_t fileSize = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<std::byte> buffer(fileSize);
            file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
            file.close();

            return buffer;
        }
    };
}

#endif