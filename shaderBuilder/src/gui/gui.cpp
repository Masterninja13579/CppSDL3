#include "gui.h"

#include "appData.h"
#include "appFunctions.h"
#include "defines.h"
#include "mainMenuBar.h"
#include "popups.h"
#include "selectorPanel.h"

#include <format>

// DEBUG
//#include <iostream>

namespace
{
    void DoNoProjectLayout(AppData& data)
    {
        ImVec2 center = ImGui::GetContentRegionAvail();
        center.x = center.x / 2;
        center.y = center.y / 2;
        ImGui::SetNextWindowPos(center, ImGuiCond_None, ImVec2(0.5f, 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
        ImGui::BeginChild("EmptyProjectPrompt", ImVec2(0, 0),
            ImGuiChildFlags_Borders |
            ImGuiChildFlags_AlwaysAutoResize |
            ImGuiChildFlags_AutoResizeX |
            ImGuiChildFlags_AutoResizeY
            );
        ImGui::PopStyleColor();
        {
            ImGui::Text("Welcome to %s version %s", APPLICATION_NAME, APPLICATION_VERSION);
            ImGui::Text("Open or create a project to get started!");
            ImGui::Separator();
            if (ImGui::Button("New", ImVec2(100, 0)))
                data.doNewProjectPopup = true;
            ImGui::SameLine();
            if (ImGui::Button("Open", ImVec2(100, 0)))
            {

            }
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }

    void DoProjectLayout(AppData& data)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

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

        ImVec2 selectionPanelSize(data.config.guiSelectionPanelWidth, 0);
        ImVec2 selectionPanelSizeMin(GUI_SELECTIONPANEL_MIN_SIZE, 0);
        ImVec2 selectionPanelSizeMax(guiRegion.x / 2, FLT_MAX);
        ImGui::SetNextWindowSizeConstraints(
            selectionPanelSizeMin,
            selectionPanelSizeMax);
        //ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
        ImGui::BeginChild("SelectionPanel", selectionPanelSize,
            ImGuiChildFlags_Borders |
            (isRegionResized ? 0 : ImGuiChildFlags_ResizeX));
        //ImGui::PopStyleColor();
        GUI::DoSelectorPanel(data);
        ImGui::EndChild();
        if (!isRegionResized)
            data.config.guiSelectionPanelWidth = ImGui::GetItemRectSize().x;

        ImVec2 selectionSidePanelSize(0, 0);
        //ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
        ImGui::SameLine();
        ImGui::BeginChild("SelectionSidePanel", selectionSidePanelSize,
            ImGuiChildFlags_Borders);
        //ImGui::PopStyleColor();
        {
            ImVec2 selectionSidePanelSize = ImGui::GetContentRegionAvail();
            ImVec2 contentPanelSize(0, -data.config.guiOutputPanelHeight);
            ImVec2 contentPanelSizeMin(0, GUI_CONTENTPANEL_MIN_SIZE);
            ImVec2 contentPanelSizeMax(FLT_MAX, selectionSidePanelSize.y - GUI_OUTPUTPANEL_MIN_SIZE);
            ImGui::SetNextWindowSizeConstraints(
                contentPanelSizeMin,
                contentPanelSizeMax);
            //ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 255, 0, 100));
            ImGui::BeginChild("ContentPanel", contentPanelSize,
                ImGuiChildFlags_Borders |
                (isRegionResized ? 0 : ImGuiChildFlags_ResizeY));
            //ImGui::PopStyleColor();

            if (ImGui::BeginTabBar("ObjectTabBar", 
                ImGuiTabBarFlags_AutoSelectNewTabs |
                ImGuiTabBarFlags_DrawSelectedOverline |
                ImGuiTabBarFlags_Reorderable |
                ImGuiTabBarFlags_TabListPopupButton |
                ImGuiTabBarFlags_NoCloseWithMiddleMouseButton))
            {
                for (int i = 0; i < data.project.shaders.size(); ++i)
                {
                    if (!data.session.shaderTab[i])
                        continue;
                    if (ImGui::BeginTabItem(
                        data.project.shaders[i].name.c_str(),
                        (bool*)&data.session.shaderTab[i],
                        ImGuiTabItemFlags_None))
                    {
                        ImGui::Text("This is the %s tab!", data.project.shaders[i].sourcePath.c_str());
                        ImGui::EndTabItem();
                    }
                }
                ImGui::EndTabBar();
            }
            ImGui::EndChild();
            if (!isRegionResized)
                data.config.guiOutputPanelHeight = selectionSidePanelSize.y - ImGui::GetItemRectSize().y;

            ImVec2 outputPanelSize(0, data.config.guiOutputPanelHeight);
            //ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 255, 100));
            ImGui::BeginChild("OutputPanel", outputPanelSize,
                ImGuiChildFlags_Borders);
            //ImGui::PopStyleColor();
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::PopStyleVar();
    }
}

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
        GUI::DoMainMenuBar(data);

        if (App::IsProjectOpen(data))
            DoProjectLayout(data);
        else
            DoNoProjectLayout(data);

        GUI::DoPopups(data);

        ImGui::End();
    }
    ImGui::PopStyleVar();
}
