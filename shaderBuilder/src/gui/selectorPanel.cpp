#include "selectorPanel.h"

namespace GUI
{
    void DoSelectorPanel(AppData& data)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
        //ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
        ImGui::BeginChild("SelectorPanelContent", ImVec2(0, 0),
            ImGuiChildFlags_AlwaysUseWindowPadding);
        ImGui::Text(data.project.name.c_str());
        ImGui::Separator();

        ImGui::SetNextItemOpen(data.session.selectorShadersOpen, ImGuiCond_FirstUseEver);
        if (ImGui::TreeNode("Shaders"))
        {
            data.session.selectorShadersOpen = true;
            for (int i = 0; i < data.project.shaders.size(); ++i)
            {
                ImGui::Text(data.project.shaders[i].name.c_str());
            }
            ImGui::Spacing();
            if (ImGui::Button("+add"))
            {
                data.doNewShaderPopup = true;
            }
            ImGui::TreePop();
        }
        else
        {
            data.session.selectorShadersOpen = false;
        }

        ImGui::SetNextItemOpen(data.session.selectorLibrariesOpen, ImGuiCond_FirstUseEver);
        if (ImGui::TreeNode("Libraries"))
        {
            data.session.selectorLibrariesOpen = true;
            ImGui::TreePop();
        }
        else
        {
            data.session.selectorLibrariesOpen = false;
        }

        ImGui::SetNextItemOpen(data.session.selectorOutputsOpen, ImGuiCond_FirstUseEver);
        if (ImGui::TreeNode("Outputs"))
        {
            data.session.selectorOutputsOpen = true;
            ImGui::TreePop();
        }
        else
        {
            data.session.selectorOutputsOpen = false;
        }

        ImGui::EndChild();
        //ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }
}
