#pragma once

#include "appData.h"
#include "models/project.h"

#include <string>

namespace App
{
    bool LoadConfig(AppData& data);
    bool SaveConfig(AppData& data);
    bool LoadSession(AppData& data);
    bool SaveSession(AppData& data);

    bool LoadProject(const std::string& path, Project& destination);
    bool SaveProject(const std::string& path, const Project& project);
    void SetSessionProject(AppData& data, const Project& project);

    std::string CalculateShaderFilepath(const std::string& path, const std::string& name);
}
