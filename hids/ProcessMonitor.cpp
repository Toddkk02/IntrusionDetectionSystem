#include "ProcessMonitor.hpp"
#include <string>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <ctime>

ProcessMonitor::ProcessMonitor(AlertQueue& queue, std::string path) 
    : alertQueue(queue), procPath(path) {}

ProcessMonitor::~ProcessMonitor() {}

void ProcessMonitor::analyzeProcess(std::string pid){
    std::ifstream cmdline("/proc/" + pid + "/cmdline");
    if(!cmdline.is_open()) return;
    
    std::string cmd;
    std::getline(cmdline, cmd);
    std::replace(cmd.begin(), cmd.end(), '\0', ' ');
    if(cmd.empty()) return;

    std::vector<std::string> suspicious = {
    // reverse shell — più specifici
    "bash -i >",           // redirect tipico di reverse shell
    "bash -i >&",          // bash reverse shell classica
    "/bin/sh -i >",
    "python -c 'import socket",   // python reverse shell
    "python3 -c 'import socket",
    "perl -e 'use Socket",
    
    // listeners/backdoor
    "ncat -l",
    "netcat -l",
    "nc -l",
    "socat TCP",
    
    // crypto miner
    "xmrig",
    "minerd",
    "cpuminer",
    
    // tool di hacking noti
    "msfconsole",
    "msfvenom",
    "hydra",
    "sqlmap",
    "linpeas",
    "linenum",
    
    // privilege escalation
    "chmod 4777",   // setuid specifico
    "chmod +s /",
};

    for(auto& s : suspicious){
        if(cmd.find(s) != std::string::npos){
            Alert a;
            a.d = Danger::CRITICAL;
            a.message = "Processo sospetto [PID " + pid + "]: " + cmd;
            a.timestamp = time(nullptr);
            alertQueue.push(a);
        }
    }
}

void ProcessMonitor::monitor(){
    for(auto& entry : std::filesystem::directory_iterator(procPath)){
        std::string name = entry.path().filename().string();
        // solo directory numeriche = processi
        if(entry.is_directory() && 
           std::all_of(name.begin(), name.end(), ::isdigit)){
            analyzeProcess(name);
        }
    }
}