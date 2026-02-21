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
        ImGui::Text(data.session.project.name.c_str());
        ImGui::Separator();

        ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
        if (ImGui::TreeNode("Shaders"))
        {
            for (int i = 0; i < data.session.project.shaders.size(); ++i)
            {
                ImGui::Text(data.session.project.shaders[i].name.c_str());
            }
            ImGui::Spacing();
            if (ImGui::Button("+add"))
            {
                data.doNewShaderPopup = true;
            }
            ImGui::TreePop();
        }

        ImGui::EndChild();
        //ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }
}
