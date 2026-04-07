#include "../headers/windowManager.h"
#include "../headers/engine.h"
#include <vector>

void WindowManager::createCommandWindow() {
    ImGui::Begin("Command line");

    float windowWidth = ImGui::GetWindowSize().x;

    float marginRatio = 0.01f;
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
        for (int i = (int)(Engine::history.size() - 1); i >= 0; i--)
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

    ImGui::SeparatorText("Commands");
    ImGui::TextWrapped("COMMAND LINE USAGE: portscan [options]\n"
    "Options:\n"
        "-h                    Show this help message\n"
        "-v                    Verbose output (show all results)\n"
        "-b                    Scan both TCP and UDP (default is TCP only)\n"
        "-u                    Scan UDP instead of TCP\n"
        "--ip <IP_PATTERN>     IP address or pattern to scan (e.g. 192.168.1.*, 127.0-10.0-24.*)\n"
        "--ports <PORTS>       Comma-separated list of ports/ranges (e.g. 80,443,1000-2000). Default is 0-1024\n"
        "--out <FILEPATH>      Filepath of the file you want the results to be exported to. Only in .html, .md, .txt formats.");


    ImGui::SeparatorText("Keybinds");
    ImGui::TextWrapped("Use up and down arrow keys to navigate through previous scans");
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

void WindowManager::createExportWindow(){
    ImGui::Begin("Export");
    
    float buttonWidth = ImGui::GetContentRegionAvail().x;
    float buttonHeight = 32.0f;
    float startX = 8.0f;
    if(Engine::exporting && Engine::selectedScan != -1){
        float windowWidth = ImGui::GetWindowSize().x;
        float marginRatio = 0.027f;
        float margin = windowWidth * marginRatio;

        float inputWidth = windowWidth - 2.0f * margin;

        ImGui::SetCursorPosX(margin);
        ImGui::SetNextItemWidth(inputWidth);

        if(ImGui::InputTextWithHint(
            "##input", 
            "Filepath...", 
            Engine::exportFilepath, 
            sizeof(Engine::exportFilepath),
            ImGuiInputTextFlags_EnterReturnsTrue
        )){
            Engine::historyMutex.lock();
            Engine::enqueueExport(std::string(Engine::exportFilepath), Engine::exportType, Engine::history[Engine::selectedScan]);
            Engine::exportFilepath[0] = '\0';
            Engine::historyMutex.unlock();

            Engine::exporting = false;
        }

        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.80f, 0.0f, 0.0f, 0.80f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.90f, 0.0f, 0.0f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(1.0f, 0.0f, 0.0f, 1.00f));
        if(ImGui::Button("Cancel", ImVec2(buttonWidth,buttonHeight))){
            Engine::exporting = false;
        }
        ImGui::PopStyleColor(3);

        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.0f, 0.30f, 0.80f, 0.80f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.30f, 0.90f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.0f, 0.30f, 1.0f, 1.00f));
        if(ImGui::Button("Export", ImVec2(buttonWidth, buttonHeight))){
            Engine::historyMutex.lock();
            Engine::enqueueExport(std::string(Engine::exportFilepath), Engine::exportType, Engine::history[Engine::selectedScan]);
            Engine::exportFilepath[0] = '\0';
            Engine::historyMutex.unlock();

            Engine::exporting = false;
        }
        ImGui::PopStyleColor(3);

    }else{
        Engine::exporting = false;
        ImGui::SetCursorPosX(startX);
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.10f, 0.35f, 0.80f, 0.80f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.10f, 0.35f, 0.85f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.10f, 0.35f, 1.0f, 1.00f));
        if(ImGui::Button("Export as .html", ImVec2(buttonWidth, buttonHeight)))
        {
            Engine::exporting = true;
            Engine::exportType = 0;
        }
        ImGui::PopStyleColor(3);
    
        ImGui::Spacing();
    
        ImGui::SetCursorPosX(startX);
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.10f, 0.70f, 0.80f, 0.80f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.10f, 0.70f, 0.85f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.10f, 0.70f, 1.0f, 1.00f));
        if(ImGui::Button("Export as .md",     ImVec2(buttonWidth, buttonHeight)))
        {
            Engine::exporting = true;
            Engine::exportType = 1;
        }
        ImGui::PopStyleColor(3);
    
        ImGui::Spacing();
    
        ImGui::SetCursorPosX(startX);
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.44f, 0.71f, 0.94f, 0.80f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.24f, 0.51f, 0.94f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.24f, 0.51f, 1.0f, 1.00f));
        if(ImGui::Button("Export as .txt",    ImVec2(buttonWidth, buttonHeight)))
        {
            Engine::exporting = true;
            Engine::exportType = 2;
        }
        ImGui::PopStyleColor(3);
    
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    
        ImGui::SetCursorPosX(startX);
        ImGui::TextDisabled("Select a format to save scan results.");
    }

    ImGui::End();
}

void WindowManager::createDockSpaceWindow(){
    // Set the dockspace window to fill the entire main viewport
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags host_flags = 
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("MyDockspace", nullptr, host_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    if (Engine::firstTime)
    {
        Engine::firstTime = false;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID dock_main_id = dockspace_id;
        ImGuiID dock_id_left, dock_id_right, dock_id_bottom, dock_id_right_bottom;

        ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, &dock_id_left, &dock_main_id);
        ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, &dock_id_right, &dock_main_id);
        ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Down, 0.30f, &dock_id_right_bottom, &dock_id_right);
        ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, &dock_id_bottom, &dock_main_id);

        ImGui::DockBuilderDockWindow("History",      dock_id_left);
        ImGui::DockBuilderDockWindow("Help",         dock_id_right);
        ImGui::DockBuilderDockWindow("Export",       dock_id_right_bottom);
        ImGui::DockBuilderDockWindow("Command line", dock_id_bottom);
        ImGui::DockBuilderDockWindow("Output",       dock_main_id);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::End();
}