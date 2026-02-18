#include "gui.h"

#include "mainMenuBar.h"
#include "popups.h"

void DoGui(AppData& data)
{
    GUI::DoMainMenuBar(data);

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    static bool open = true;
    static ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoTitleBar;
    if (ImGui::Begin("WorkArea", &open, flags))
    {
        GUI::DoPopups(data);

        ImGui::End();
    }
}
