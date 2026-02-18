#include "popups.h"

namespace
{
    void HandleNewProjectModal(AppData& data)
    {
        static bool initialized = false;

        if (data.openNewProjectPopup)
            ImGui::OpenPopup("New Project");
        data.openNewProjectPopup = false;

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

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
                Project project(data.newProjectName);
                data.session.selectedProject = data.session.openProjects.size();
                data.session.openProjects.push_back(project);
                ImGui::CloseCurrentPopup();
                initialized = false;
            }
            ImGui::EndPopup();
        }
    }
}

namespace GUI
{
    void DoPopups(AppData& data)
    {
        HandleNewProjectModal(data);
    }
}
