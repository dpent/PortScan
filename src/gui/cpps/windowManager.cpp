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
        Engine::enqueueCommand(Engine::currentCommand);
        Engine::currentCommand[0] = '\0';
    }

    Scan::activeScansMutex.lock();
    if(Scan::numActiveScans!=0){

        Engine::spinnerTimer += ImGui::GetIO().DeltaTime;

        // Only update spinner when enough time has passed
        if (Engine::spinnerTimer >= Engine::spinnerInterval) {
            Engine::spinnerIndex = (Engine::spinnerIndex + 1) % 4;
            Engine::spinnerTimer = 0.0f;
        }

        std::string str = "There are currently " 
                + std::to_string(Scan::numActiveScans) 
                + " scans queued or running " 
                + std::string(1, Engine::spinner[Engine::spinnerIndex]);

        ImGui::Text("%s",str.c_str());
    }
    Scan::activeScansMutex.unlock();

    ImGui::End();
}

void WindowManager::createPastScansWindow(){
    ImGui::Begin("History");

    ImVec2 listBoxSize = ImGui::GetContentRegionAvail();

    if (ImGui::BeginListBox("##list", listBoxSize))
    {
        Engine::historyMutex.lock();
        for (int i = Engine::history.size() - 1; i >= 0; i--)
        {
            const bool is_selected = (Engine::selectedScan == i);
            if (ImGui::Selectable(Engine::history[i]->publicName.c_str(), is_selected))
                Engine::selectedScan = i;

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        Engine::historyMutex.unlock();
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
        Engine::historyMutex.lock();
        Scan* selected = Engine::history[Engine::selectedScan];
        ImGui::TextWrapped("%s", selected->outputString.c_str());
        Engine::historyMutex.unlock();
    }
    ImGui::End();
}