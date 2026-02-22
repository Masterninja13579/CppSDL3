#include "appFunctions.h"

#include "core.h"

#include "utils/fileIO.h"

#include <filesystem>

namespace
{
    void RefreshWindowTitle(AppData& data)
    {
        std::stringstream ss;
        ss << APPLICATION_NAME_AND_VERSION;
        if (data.project.name.size() > 0)
        {
            ss << " - " << data.project.name;
            if (data.isProjectDirty)
                ss << "*";
        }
        data.window->SetName(ss.str());
    }
}

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

    bool IsProjectOpen(AppData& data)
    {
        return data.session.projectName.size() > 0;
    }
    bool CreateProject(AppData& data)
    {
        data.project = Project(data.newProjectName);
        data.session.projectName = data.project.name;
        std::stringstream ss;
        ss << data.config.projectsPath << data.newProjectName << "/";
        if (!ensureDirectories(ss.str()))
            return false;
        RefreshWindowTitle(data);
        return SaveProject(data);
    }
    bool LoadProject(AppData& data)
    {
        std::stringstream ss;
        ss << data.config.projectsPath << data.session.projectName << "/";
        ss << data.session.projectName << PROJECT_EXTENSION;
        return readJsonFile<Project>(ss.str(), data.project);
    }
    bool SaveProject(AppData& data)
    {
        std::stringstream ss;
        ss << data.config.projectsPath << data.session.projectName << "/";
        ss << data.session.projectName << PROJECT_EXTENSION;
        bool result = writeJsonFile<Project>(ss.str(), data.project, true);
        if (result)
            data.isProjectDirty = false;
        RefreshWindowTitle(data);
        return result;
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
    bool IsShaderNameAvailable(AppData& data, const std::string& name)
    {
        for (int i = 0; i < data.project.shaders.size(); ++i)
            if (data.project.shaders[i].name == name)
                return false;
        return true;
    }
    bool CreateShader(AppData& data)
    {
        std::string name(data.newShaderName);
        std::string path = CalculateShaderFilepath(data.config.shadersPath, name);
        const char* types[] = SHADER_TYPES;
        std::string type = types[data.session.newShaderTypeSelection];
        Shader shader(name, path, type);
        data.project.shaders.push_back(shader);
        data.session.shaderTab.push_back((char)true);
        data.isProjectDirty = true;
        RefreshWindowTitle(data);
        return true;
    }
}


