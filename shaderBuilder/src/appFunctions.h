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

    std::string CalculateProjectDirectoryPath(AppData& data);
    std::string CalculateProjectFilePath(AppData& data);
    bool IsProjectOpen(AppData& data);
    bool CreateProject(AppData& data);
    bool LoadProject(AppData& data);
    bool SaveProject(AppData& data);

    std::string CalculateShaderDirectoryPath(AppData& data, const std::string& name);
    std::string CalculateShaderSourcePath(AppData& data, const std::string& name);
    std::string CalculateShaderVaryingPath(AppData& data, const std::string& name);
    bool IsShaderNameAvailable(AppData& data, const std::string& name);
    bool CreateShader(AppData& data);
    bool OpenShaderTab(AppData& data, int shaderIndex);
    bool LoadShaderSource(AppData& data, int shaderIndex);
    bool SaveShaderSource(AppData& data, int shaderIndex);
}
