#include "appFunctions.h"

#include "core.h"

#include "utils/fileIO.h"

#include <filesystem>

namespace App
{
    bool LoadConfig(AppData& data)
    {
        return readJsonFile<Config>(APPLICATION_CONFIG_FILEPATH, data.config);
    }
    bool SaveConfig(AppData& data)
    {
        return writeJsonFile<Config>(APPLICATION_CONFIG_FILEPATH, data.config, true);
    }
    bool LoadSession(AppData& data)
    {
        return readJsonFile<Session>(APPLICATION_SESSION_FILEPATH, data.session);
    }
    bool SaveSession(AppData& data)
    {
        return writeJsonFile<Session>(APPLICATION_SESSION_FILEPATH, data.session, true);
    }

    bool LoadProject(const std::string& path, Project& destination)
    {
        return readJsonFile<Project>(path, destination);
    }
    bool SaveProject(const std::string& path, Project& project)
    {
        return writeJsonFile<Project>(path, project, true);
    }
    void SetSessionProject(AppData& data, const Project& project)
    {
        if (&data.session.project != &project)
        data.session.project = project;
        std::stringstream ss;
        ss << APPLICATION_NAME_AND_VERSION;
        if (data.session.project.name.size() > 0)
        ss << " - " << data.session.project.name;
        if (data.session.project.dirty)
        ss << "*";
        data.window->SetName(ss.str());
    }

    std::string CalculateShaderFilepath(const std::string& path, const std::string& name)
    {
        for (int i = 0; i < 100; ++i)
        {
            std::stringstream ss;
            ss << name;
            if (i > 0)
                ss << " (" << i << ")";
            ss << SHADER_EXTENSION;
            std::filesystem::path fullPath(path);
            fullPath.append(ss.str());
            if (!std::filesystem::exists(fullPath))
                return fullPath;
        }
        return "";
    }
}


