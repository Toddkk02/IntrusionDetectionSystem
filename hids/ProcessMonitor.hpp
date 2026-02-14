#pragma once
#include <string>
#include <vector>
#include "../core/AlertQueue.hpp"

class ProcessMonitor {    
private:
    std::string procPath;
    AlertQueue& alertQueue; 
    
    
    void analyzeProcess(std::string pid);

public:
    
    ProcessMonitor(AlertQueue& queue, std::string path = "/proc");
    ~ProcessMonitor();

    
    void monitor();
};