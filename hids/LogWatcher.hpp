#pragma once
#include <sys/inotify.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include "../core/AlertQueue.hpp"

class LogWatcher {
private:
    int inotifyFd;
    int watchFd;
    std::ifstream logFile;
    std::streampos lastPos;
    AlertQueue& queue;  
public:
    LogWatcher(std::string filepath, AlertQueue& queue);  
    ~LogWatcher();
    void watch();
    void readNewLines();
};