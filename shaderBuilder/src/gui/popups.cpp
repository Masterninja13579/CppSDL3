#include "popups.h"

#include "appFunctions.h"

// DEBUG
#include <iostream>

namespace
{
    void HandleNewProjectModal(AppData& data)
    {
        static bool initialized = false;

        if (data.doNewProjectPopup)
            ImGui::OpenPopup("New Project");
        data.doNewProjectPopup = false;

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

        if (ImGui::BeginPopupModal("New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (!initialized)
            {
                strcpy(data.newProjectName, PROJECT_NAME_DEFAULT);
                initialized = true;
            }

            ImGui::Text("Project Name");
            ImGui::SameLine();
            ImGui::InputText("##NewProject", data.newProjectName, PROJECT_NAME_SIZE);
            ImGui::Separator();
            if (ImGui::Button("Cancel", ImVec2(128, 0)))
            {
                ImGui::CloseCurrentPopup();

                initialized = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Create", ImVec2(128, 0)))
            {
                App::CreateProject(data);
                ImGui::CloseCurrentPopup();
                initialized = false;
            }
            ImGui::EndPopup();
        }

        ImGui::PopStyleVar();
    }

    std::string BuildShaderErrorText(const std::string& name)
    {
        std::stringstream ss;
        ss << "Shader '" << name << "' already exists!";
        return ss.str();
    }
    std::string BuildShaderOutputText(const std::string& path)
    {
        std::stringstream ss;
        ss << "Shader will be saved as '" << path << "'";
        return ss.str();
    }
    void HandleNewShaderModal(AppData& data)
    {
        static bool initialized = false;
        static std::string shaderOutputPath = "";
        static std::string shaderOutputText = "";
        static std::string shaderErrorText = "";
        static bool shaderNameAvailable = false;
        bool selectCreate = false;
        bool selectImport = false;

        if (data.doNewShaderPopup)
            ImGui::OpenPopup("New Shader");
        data.doNewShaderPopup = false;

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

        if (ImGui::BeginPopupModal("New Shader", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (!initialized)
            {
                strcpy(data.newShaderName, SHADER_NAME_DEFAULT);
                shaderOutputPath = App::CalculateShaderFilepath(
                    data.config.shadersPath, 
                    data.newShaderName);
                shaderOutputText = BuildShaderOutputText(shaderOutputPath);
                shaderNameAvailable = App::IsShaderNameAvailable(data, data.newShaderName);
                if (!shaderNameAvailable)
                    shaderErrorText = BuildShaderErrorText(data.newShaderName);
                selectCreate = data.session.newShaderTabCreate;
                selectImport = data.session.newShaderTabImport;
                initialized = true;
            }

            if (ImGui::BeginTabBar("NewShaderTabs", 
                ImGuiTabBarFlags_DrawSelectedOverline |
                ImGuiTabBarFlags_NoCloseWithMiddleMouseButton |
                ImGuiTabBarFlags_NoTabListScrollingButtons))
            {
                if (ImGui::BeginTabItem("Create", nullptr,
                    selectCreate ? ImGuiTabItemFlags_SetSelected : 0))
                {
                    data.session.newShaderTabCreate = true;
                    data.session.newShaderTabImport = false;
                    ImGui::Text("Shader Name");
                    ImGui::SameLine();
                    ImGui::InputText("##NewShader", data.newShaderName, SHADER_NAME_SIZE,
                        ImGuiInputTextFlags_CallbackEdit,
                        [](ImGuiInputTextCallbackData* callbackData){
                            AppData* data = (AppData*)callbackData->UserData;
                            shaderOutputPath = App::CalculateShaderFilepath(
                                data->config.shadersPath, 
                                callbackData->Buf);
                            shaderOutputText = BuildShaderOutputText(shaderOutputPath);
                            shaderNameAvailable = App::IsShaderNameAvailable(*data, callbackData->Buf);
                            if (!shaderNameAvailable)
                                shaderErrorText = BuildShaderErrorText(callbackData->Buf);
                            return 0;
                        },
                        &data);
                    if (!shaderNameAvailable)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                        ImGui::PushStyleVarX(ImGuiStyleVar_FramePadding, 30);
                        ImGui::Text(shaderErrorText.c_str());
                        ImGui::PopStyleVar();
                        ImGui::PopStyleColor();
                    }
                    ImGui::Text(shaderOutputText.c_str());
                    ImGui::Text("Type: ");
                    int index = 0;
                    for (auto& type : SHADER_TYPES)
                    {
                        ImGui::SameLine();
                        ImGui::RadioButton(type, &data.session.newShaderTypeSelection, index);
                        index++;
                    }
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Import", nullptr,
                    selectImport ? ImGuiTabItemFlags_SetSelected : 0))
                {
                    data.session.newShaderTabCreate = false;
                    data.session.newShaderTabImport = true;
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            bool areSettingsValid = true;
            if (data.session.newShaderTabCreate)
                areSettingsValid = shaderOutputPath.size() > 0 && 
                                   data.newShaderName[0] != 0;
            if (data.session.newShaderTabImport)
                areSettingsValid = false;

            ImGui::Separator();
            if (ImGui::Button("Cancel", ImVec2(128, 0)))
            {
                ImGui::CloseCurrentPopup();
                initialized = false;
            }
            ImGui::SameLine();
            const std::string okButtonName = data.session.newShaderTabCreate ? "Create" : "Import";
            ImGui::BeginDisabled(!areSettingsValid);
            if (ImGui::Button(okButtonName.c_str(), ImVec2(128, 0)))
            {
                if (data.session.newShaderTabCreate)
                    App::CreateShader(data);
                ImGui::CloseCurrentPopup();
                initialized = false;
            }
            ImGui::EndDisabled();
            ImGui::EndPopup();
        }

        ImGui::PopStyleVar();
    }
}

namespace GUI
{
    void DoPopups(AppData& data)
    {
        HandleNewProjectModal(data);
        HandleNewShaderModal(data);
    }
}
