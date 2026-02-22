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

    bool IsProjectOpen(AppData& data);
    bool CreateProject(AppData& data);
    bool LoadProject(AppData& data);
    bool SaveProject(AppData& data);

    std::string CalculateShaderFilepath(const std::string& path, const std::string& name);
    bool IsShaderNameAvailable(AppData& data, const std::string& name);
    bool CreateShader(AppData& data);
}
