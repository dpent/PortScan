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
    Scan* selected = nullptr;
    if(Engine::selectedScan!=-1){
        Engine::historyMutex.lock();
        selected = Engine::history[Engine::selectedScan];
        Engine::historyMutex.unlock();
    }

    if(selected){
        
        ImGui::SeparatorText("Output");
        ImGui::TextWrapped("%s", selected->outputString.c_str());

        ImGui::SeparatorText("IPv4s scanned");

        float item_min_width = 80.0f; // tweak this
        float available_width = ImGui::GetContentRegionAvail().x;

        int columns = (int)(available_width / item_min_width);
        if (columns < 1) columns = 1;

        if (ImGui::BeginTable("DynamicTable", columns, ImGuiTableFlags_SizingStretchSame))
        {
            for (int i = 0; i < selected->ipsScanned.size(); i++)
            {
                ImGui::TableNextColumn();
                ImGui::Text("%s", selected->ipsScanned[i].c_str());
            }

            ImGui::EndTable();
        }

        ImGui::SeparatorText("Ports scanned per IPv4");

        item_min_width = 40.0f; // tweak this

        columns = (int)(available_width / item_min_width);
        if (columns < 1) columns = 1;

        if (ImGui::BeginTable("PortTable", columns, ImGuiTableFlags_SizingStretchSame))
        {
            for (int i = 0; i < selected->portsScanned.size(); i++)
            {
                ImGui::TableNextColumn();
                ImGui::Text("%d", selected->portsScanned[i]);
            }

            ImGui::EndTable();
        }

    }else{
        
        ImGui::SeparatorText("Output");
        ImGui::TextWrapped("Scan results will be displayed here.");

        ImGui::SeparatorText("IPv4s scanned");
        ImGui::TextWrapped("IPv4 addresses scanned will be displayed here.");

        ImGui::SeparatorText("Ports scanned per IPv4");
        ImGui::TextWrapped("Ports per address will be displayed here.");
    }

    ImGui::End();
}