#ifndef __FILEUTILS_H__
#define __FILEUTILS_H__

#include "common/Singleton.h"

#include <fstream>
#include <string>
#include <sstream>

namespace OGLRenderer::Common
{
    class FileUtils : public Singleton<FileUtils>
    {
    public:
        std::string ReadFile(std::string filePath)
        {
            std::ifstream fileStream(filePath);
            if (!fileStream.is_open())
                return std::string();
            std::stringstream fileStrStream;
            fileStrStream << fileStream.rdbuf();
            return fileStrStream.str();
        }
    };
}

#endif