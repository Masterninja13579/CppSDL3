#include "mainMenuBar.h"

namespace GUI
{
    void DoMainMenuBar(AppData& data)
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "CTRL+N")) {}
                if (ImGui::MenuItem("Open", "CTRL+O")) {}
                if (ImGui::MenuItem("Save", "CTRL+S", false, false)) {}
                if (ImGui::MenuItem("Save As", "CTRL+SHIFT+S", false, false)) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "CTRL+Q")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {} // Disabled item
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "CTRL+X")) {}
                if (ImGui::MenuItem("Copy", "CTRL+C")) {}
                if (ImGui::MenuItem("Paste", "CTRL+V")) {}
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
}
