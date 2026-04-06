#include "../headers/windowManager.h"
#include "../headers/engine.h"
#include <vector>

void WindowManager::createCommandWindow() {
    ImGui::Begin("Command line");

    float windowWidth = ImGui::GetWindowSize().x;

    float marginRatio = 0.1f; // 10% on each side
    float margin = windowWidth * marginRatio;

    float inputWidth = windowWidth - 2.0f * margin;

    ImGui::SetCursorPosX(margin);
    ImGui::SetNextItemWidth(inputWidth);

    if(ImGui::InputTextWithHint(
        "##input", 
        "Type here...", 
        Engine::currentCommand, 
        sizeof(Engine::currentCommand),
        ImGuiInputTextFlags_EnterReturnsTrue
    )){
        Engine::executeCommand(Engine::currentCommand);
        Engine::currentCommand[0] = '\0';
    }

    ImGui::End();
}

void WindowManager::createPastScansWindow(){
    ImGui::Begin("History");

    ImVec2 listBoxSize = ImGui::GetContentRegionAvail();

    if (ImGui::BeginListBox("##list", listBoxSize))
    {
        for (int i = Engine::history.size() - 1; i >= 0; i--)
        {
            const bool is_selected = (Engine::selectedScan == i);
            if (ImGui::Selectable(Engine::history[i]->command.c_str(), is_selected))
                Engine::selectedScan = i;

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }
    ImGui::End();
}

void WindowManager::createHelpWindow(){
    ImGui::Begin("Help");

    ImGui::TextWrapped("COMMAND LINE USAGE: portscan [options]\n"
    "Options:\n"
        "-h                    Show this help message\n"
        "-v                    Verbose output (show all results)\n"
        "-b                    Scan both TCP and UDP (default is TCP only)\n"
        "-u                    Scan UDP instead of TCP\n"
        "--ip <IP_PATTERN>     IP address or pattern to scan (e.g. 192.168.1.*, 127.0-10.0-24.*)\n"
        "--ports <PORTS>       Comma-separated list of ports/ranges (e.g. 80,443,1000-2000). Default is 0-1024\n");

    ImGui::End();
}

void WindowManager::createOutputWindow(){
    ImGui::Begin("Output");
    if(Engine::selectedScan==-1){

        ImGui::TextWrapped("Scan results will be displayed here.");
    }else{
        Scan* selected = Engine::history[Engine::selectedScan];
        ImGui::TextWrapped("%s", selected->outputString.c_str());
    }
    ImGui::End();
}