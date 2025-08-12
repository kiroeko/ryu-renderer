#include "app/AppSettings.h"

#include <iostream>
#include <fstream>

#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

namespace RyuRenderer::App
{
    AppSettings::AppSettings()
    {
        std::ifstream ifs(appSettingsJsonFilePath);
        if (!ifs.is_open())
        {
            return;
        }

        rapidjson::IStreamWrapper isw(ifs);
        rapidjson::Document doc;
        doc.ParseStream(isw);
        if (doc.HasParseError())
        {
            std::cerr << "Failed to load app settings file on: " << appSettingsJsonFilePath << std::endl;
            return;
        }

        if (!doc.IsObject())
        {
            std::cerr << "Failed to parse app settings file: " << appSettingsJsonFilePath << "as json object." << std::endl;
            return;
        }

        if (doc.HasMember("WindowName") && doc["WindowName"].IsString())
        {
            WindowName = doc["WindowName"].GetString();
        }

        if (doc.HasMember("WindowIconPath") && doc["WindowIconPath"].IsString())
        {
            WindowIconPath = doc["WindowIconPath"].GetString();
        }

        if (doc.HasMember("WindowWidth") && doc["WindowWidth"].IsInt())
        {
            WindowWidth = doc["WindowWidth"].GetInt();
        }

        if (doc.HasMember("WindowHeight") && doc["WindowHeight"].IsInt())
        {
            WindowHeight = doc["WindowHeight"].GetInt();
        }

        if (doc.HasMember("VSyncInterval") && doc["VSyncInterval"].IsInt())
        {
            VSyncInterval = doc["VSyncInterval"].GetInt();
        }

        if (doc.HasMember("MSAA") && doc["MSAA"].IsInt())
        {
            MSAA = doc["MSAA"].GetInt();
        }

        if (doc.HasMember("HideCursor") && doc["HideCursor"].IsBool())
        {
            HideCursor = doc["HideCursor"].GetBool();
        }

        if (doc.HasMember("LockCursorToCenter") && doc["LockCursorToCenter"].IsBool())
        {
            LockCursorToCenter = doc["LockCursorToCenter"].GetBool();
        }
    }
}