#include "appFunctions.h"

#include "core.h"

#include "constants.h"
#include "utils/fileIO.h"

#include <filesystem>

namespace App
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
        bool result = readJsonFile<Session>(APPLICATION_SESSION_FILEPATH, data.session);
        if (!result)
            return false;
        if (!IsProjectOpen(data))
            return true;
        if (!LoadProject(data))
        {
            data.session = Session();
            return true;
        }
        int shaderCount = data.project.shaders.size();
        data.shaderLoaded.resize(shaderCount);
        data.shaderDirty.resize(shaderCount);
        data.shaderSource.resize(shaderCount);
        data.shaderVaryingSource.resize(shaderCount);
        for (int i = 0; i < data.session.openTabs.size(); ++i)
        {
            int index = data.session.openTabs[i].index;
            switch (data.session.openTabs[i].source)
            {
                case TAB_SOURCE_SHADER:
                {
                    if (index >= data.project.shaders.size())
                    {
                        data.session.openTabs.erase(data.session.openTabs.begin() + i);
                        --i;
                        continue;
                    }
                    data.shaderLoaded[index] = LoadShaderSource(data, index);
                    break;
                }
                case TAB_SOURCE_LIBRARY: break;
                case TAB_SOURCE_OUTPUT: break;
                default: break;
            }
        }
        return true;
    }
    bool SaveSession(AppData& data)
    {
        return writeJsonFile<Session>(APPLICATION_SESSION_FILEPATH, data.session, true);
    }

    std::string CalculateProjectDirectoryPath(AppData& data)
    {
        std::stringstream ss;
        ss << data.config.projectsPath << data.session.projectName << "/";
        return ss.str();
    }
    std::string CalculateProjectFilePath(AppData& data)
    {
        std::stringstream ss;
        ss << CalculateProjectDirectoryPath(data);
        ss << data.session.projectName << PROJECT_EXTENSION;
        return ss.str();
    }
    bool IsProjectOpen(AppData& data)
    {
        return data.session.projectName.size() > 0;
    }
    bool CreateProject(AppData& data)
    {
        data.project = Project(data.newProjectName);
        data.session.projectName = data.project.name;
        if (!ensureDirectories(CalculateProjectDirectoryPath(data)))
            return false;
        RefreshWindowTitle(data);
        return SaveProject(data);
    }
    bool LoadProject(AppData& data)
    {
        return readJsonFile<Project>(CalculateProjectFilePath(data), data.project);
    }
    bool SaveProject(AppData& data)
    {
        for (int i = 0; i < data.project.shaders.size(); ++i)
            if (data.shaderDirty[i])
                SaveShaderSource(data, i);
        bool result = writeJsonFile<Project>(CalculateProjectFilePath(data), data.project, true);
        if (result)
            data.isProjectDirty = false;
        RefreshWindowTitle(data);
        return result;
    }

    std::string CalculateShaderDirectoryPath(AppData& data, const std::string& name)
    {
        std::stringstream ss;
        ss << CalculateProjectDirectoryPath(data) << data.config.shadersPath;
        ss << name << "/";
        return ss.str();
    }
    std::string CalculateShaderSourcePath(AppData& data, const std::string& name)
    {
        std::stringstream ss;
        ss << CalculateShaderDirectoryPath(data, name);
        ss << name << SHADER_EXTENSION;
        return ss.str();
    }
    std::string CalculateShaderVaryingPath(AppData& data, const std::string& name)
    {
        std::stringstream ss;
        ss << CalculateShaderDirectoryPath(data, name) << SHADER_VARYING_FILENAME;
        return ss.str();
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
        std::string directoryPath = CalculateShaderDirectoryPath(data, name);
        if (!ensureDirectories(directoryPath))
            return false;
        std::string sourcePath = CalculateShaderSourcePath(data, name);
        std::string varyingPath = CalculateShaderVaryingPath(data, name);
        const char* types[] = SHADER_TYPES;
        std::string type = types[data.session.newShaderTypeSelection];
        std::string sourceText = SHADER_SOURCE_DEFAULT.at(type);
        std::string varyingText = SHADER_VARYING_DEFAULT.at(type);
        Shader shader(name, type);
        data.project.shaders.push_back(shader);
        data.shaderLoaded.push_back(true);
        data.shaderDirty.push_back(true);
        data.shaderSource.push_back(sourceText);
        data.shaderVaryingSource.push_back(varyingText);
        data.isProjectDirty = true;
        RefreshWindowTitle(data);
        SaveShaderSource(data, data.project.shaders.size() - 1);
        OpenShaderTab(data, data.project.shaders.size() - 1);
        return true;
    }
    bool OpenShaderTab(AppData& data, int shaderIndex)
    {
        for (int i = 0; i < data.session.openTabs.size(); ++i)
        {
            if (data.session.openTabs[i].source == TAB_SOURCE_SHADER &&
                data.session.openTabs[i].index == shaderIndex)
            {
                data.selectNextTab = i;
                return true;
            }
        }
        data.session.openTabs.push_back(Tab(TAB_SOURCE_SHADER, shaderIndex));
        if (!data.shaderLoaded[shaderIndex])
            return LoadShaderSource(data, shaderIndex);
        return true;
    }
    bool LoadShaderSource(AppData& data, int shaderIndex)
    {
        std::string shaderName = data.project.shaders[shaderIndex].name;
        std::string sourcePath = CalculateShaderSourcePath(data, shaderName);
        std::string varyingPath = CalculateShaderVaryingPath(data, shaderName);
        bool loadedSource = readTextFile(sourcePath, data.shaderSource[shaderIndex]);
        bool loadedVarying = readTextFile(varyingPath, data.shaderVaryingSource[shaderIndex]);
        return loadedSource && loadedVarying;
    }
    bool SaveShaderSource(AppData& data, int shaderIndex)
    {
        std::string shaderName = data.project.shaders[shaderIndex].name;
        std::string sourcePath = CalculateShaderSourcePath(data, shaderName);
        std::string varyingPath = CalculateShaderVaryingPath(data, shaderName);
        bool savedSource = writeTextFile(sourcePath, data.shaderSource[shaderIndex], true);
        bool savedVarying = writeTextFile(varyingPath, data.shaderVaryingSource[shaderIndex], true);
        data.shaderDirty[shaderIndex] = false;
        return savedSource && savedVarying;
    }
}


