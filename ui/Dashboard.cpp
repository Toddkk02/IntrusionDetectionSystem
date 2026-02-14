#include "Dashboard.hpp"
#include <deque>
#include "../core/Config.hpp"
#include <fstream>
#include <algorithm>
Dashboard::Dashboard(AlertQueue& q) : queue(q), window(nullptr), renderer(nullptr), running(false) {
    startTime = time(nullptr);
};

void Dashboard::init(){
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "IDS dashboard", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_SHOWN
    );

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForSDLRenderer(window,renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

}
Dashboard::~Dashboard(){
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


void Dashboard::exportCSV(){
    std::ofstream file("ids_export.csv");
    file << "Timestamp,Severity,Message\n";
    
    for(auto& alert : alertLog){
        char timebuf[32];
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&alert.timestamp));
        
        std::string severity;
        switch(alert.d){
            case Danger::INFO:     severity = "INFO";     break;
            case Danger::WARNING:  severity = "WARNING";  break;
            case Danger::ALERT:    severity = "ALERT";    break;
            case Danger::CRITICAL: severity = "CRITICAL"; break;
        }
        
        std::string msg = alert.message;
        std::replace(msg.begin(), msg.end(), ',', ';');
        
        file << timebuf << "," << severity << "," << msg << "\n";
    }
    file.close();
}

void Dashboard::render(){
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("IDS Dashboard", nullptr, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove);

    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 1.0f), "IDS - Intrusion Detection System");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "| Status: RUNNING");
    ImGui::Separator();
    time_t now = time(nullptr);
int uptime = (int)(now - startTime);
int hours   = uptime / 3600;
int minutes = (uptime % 3600) / 60;
int seconds = uptime % 60;

// conta per severità
int countInfo = 0, countWarning = 0, countAlert = 0, countCritical = 0;
for(auto& a : alertLog){
    switch(a.d){
        case Danger::INFO:     countInfo++;     break;
        case Danger::WARNING:  countWarning++;  break;
        case Danger::ALERT:    countAlert++;    break;
        case Danger::CRITICAL: countCritical++; break;
    }
}

// pannello statistiche
    ImGui::BeginChild("stats", ImVec2(0, 60), true);
    
    ImGui::Text("Uptime: %02d:%02d:%02d", hours, minutes, seconds);
    ImGui::SameLine(200);
    ImGui::Text("Totale alert: %d", (int)alertLog.size());
    ImGui::SameLine(400);
    ImGui::TextColored(ImVec4(0.5f,0.5f,1.0f,1.0f),  "INFO: %d",     countInfo);
    ImGui::SameLine(500);
    ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f),  "WARNING: %d",  countWarning);
    ImGui::SameLine(620);
    ImGui::TextColored(ImVec4(1.0f,0.5f,0.0f,1.0f),  "ALERT: %d",    countAlert);
    ImGui::SameLine(720);
    ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1.0f),  "CRITICAL: %d", countCritical);
    
    ImGui::EndChild();
    ImGui::SameLine(900);
    if(ImGui::Button("Export CSV")){
        exportCSV();
    }
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "-> ids_export.csv");
    ImGui::Separator();

    Alert a;
while(queue.pop(a)){
    alertLog.push_back(a);
    // mantieni solo gli ultimi 1000 alert
    if(alertLog.size() > Config::MAX_ALERT_LOG)
    alertLog.pop_front();
}

    if(ImGui::BeginTable("alerts", 3, 
        ImGuiTableFlags_Borders | 
        ImGuiTableFlags_RowBg | 
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_Resizable,
        ImVec2(0, 0))){

        ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("Severity",  ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Message",   ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();


        for(auto& alert : alertLog){
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            char timebuf[32];
            strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&alert.timestamp));
            ImGui::Text("%s", timebuf);

            ImGui::TableSetColumnIndex(1);
            switch(alert.d){
                case Danger::INFO:
                    ImGui::TextColored(ImVec4(0.5f,0.5f,1.0f,1.0f), "INFO"); break;
                case Danger::WARNING:
                    ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "WARNING"); break;
                case Danger::ALERT:
                    ImGui::TextColored(ImVec4(1.0f,0.5f,0.0f,1.0f), "ALERT"); break;
                case Danger::CRITICAL:
                    ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1.0f), "CRITICAL"); break;
            }

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", alert.message.c_str());
        }

        ImGui::EndTable();
    }

    ImGui::End();

    ImGui::Render();
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}
void Dashboard::run(){
    running = true;
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            ImGui_ImplSDL2_ProcessEvent(&event);
            if(event.type == SDL_QUIT)
                running = false;
        }
        render();
    }
}