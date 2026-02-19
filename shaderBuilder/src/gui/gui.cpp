#include "gui.h"

#include "defines.h"
#include "mainMenuBar.h"
#include "popups.h"

// DEBUG
#include <iostream>

void DoGui(AppData& data)
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    static bool open = true;
    
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Begin("ShaderBuilderGui", &open,
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_MenuBar))
    {
        ImGui::PopStyleVar();

        GUI::DoMainMenuBar(data);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        // Check if the window is being resized
        bool isRegionResized = false;
        ImVec2 guiRegion = ImGui::GetContentRegionAvail();
        static ImVec2 guiRegionPrevious = guiRegion;
        if (guiRegion.x != guiRegionPrevious.x ||
            guiRegion.y != guiRegionPrevious.y)
        {
            guiRegionPrevious = guiRegion;
            isRegionResized = true;
        }

        ImVec2 selectionPanelSize(data.guiSelectionPanelWidth, 0);
        ImVec2 selectionPanelSizeMin(GUI_SELECTIONPANEL_MIN_SIZE, 0);
        ImVec2 selectionPanelSizeMax(guiRegion.x / 2, FLT_MAX);
        ImGui::SetNextWindowSizeConstraints(
            selectionPanelSizeMin,
            selectionPanelSizeMax);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
        ImGui::BeginChild("SelectionPanel", selectionPanelSize,
            ImGuiChildFlags_Borders |
            (isRegionResized ? 0 : ImGuiChildFlags_ResizeX));
        ImGui::EndChild();
        if (!isRegionResized)
            data.guiSelectionPanelWidth = ImGui::GetItemRectSize().x;
        ImGui::PopStyleColor();

        ImVec2 selectionSidePanelSize(0, 0);
        //ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
        ImGui::SameLine();
        ImGui::BeginChild("SelectionSidePanel", selectionSidePanelSize,
            ImGuiChildFlags_Borders);
        {
            ImVec2 selectionSidePanelSize = ImGui::GetContentRegionAvail();
            ImVec2 contentPanelSize(0, -data.guiOutputPanelHeight);
            ImVec2 contentPanelSizeMin(0, GUI_CONTENTPANEL_MIN_SIZE);
            ImVec2 contentPanelSizeMax(FLT_MAX, selectionSidePanelSize.y - GUI_OUTPUTPANEL_MIN_SIZE);
            ImGui::SetNextWindowSizeConstraints(
                contentPanelSizeMin,
                contentPanelSizeMax);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 255, 0, 100));
            ImGui::BeginChild("ContentPanel", contentPanelSize,
                ImGuiChildFlags_Borders |
                (isRegionResized ? 0 : ImGuiChildFlags_ResizeY));
            {
                
            }
            ImGui::EndChild();
            if (!isRegionResized)
                data.guiOutputPanelHeight = selectionSidePanelSize.y - ImGui::GetItemRectSize().y;
            std::cout << data.guiOutputPanelHeight << "\n";
            ImGui::PopStyleColor();

            ImVec2 outputPanelSize(0, data.guiOutputPanelHeight);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 255, 100));
            ImGui::BeginChild("OutputPanel", outputPanelSize,
                ImGuiChildFlags_Borders);
            ImGui::EndChild();
            ImGui::PopStyleColor();
        }
        ImGui::EndChild();
        //ImGui::PopStyleColor();

        ImGui::PopStyleVar();

        GUI::DoPopups(data);

        ImGui::End();
    }

    ImGui::PopStyleVar();
}
