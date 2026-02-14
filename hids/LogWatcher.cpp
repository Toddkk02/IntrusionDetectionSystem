#include <sys/inotify.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include "LogWatcher.hpp"

LogWatcher::LogWatcher(std::string path, AlertQueue& q) : queue(q) {
    logFile.open(path);
    logFile.seekg(0, std::ios::end);
    lastPos = logFile.tellg();
    inotifyFd = inotify_init();
    watchFd = inotify_add_watch(inotifyFd, path.c_str(), IN_MODIFY);
}

LogWatcher::~LogWatcher(){
    logFile.close();
    close(inotifyFd);
    close(watchFd);

}

void LogWatcher::readNewLines(){
    logFile.seekg(lastPos);
    std::string line;
    while(std::getline(logFile, line)){
        Alert a;
        a.timestamp = time(nullptr);
        a.message = line;

        // classifica in base al contenuto
        if(line.find("Failed password") != std::string::npos){
            a.d = Danger::WARNING;
        }
        else if(line.find("BREAK-IN") != std::string::npos ||
                line.find("Invalid user") != std::string::npos){
            a.d = Danger::CRITICAL;
        }
        else if(line.find("Accepted password") != std::string::npos ||
                line.find("Accepted publickey") != std::string::npos){
            a.d = Danger::ALERT;
        }
        else if(line.find("sudo") != std::string::npos){
            a.d = Danger::WARNING;
        }
        else{
            a.d = Danger::INFO;
        }

        queue.push(a);
    }
    logFile.clear();
    lastPos = logFile.tellg();
}

void LogWatcher::watch(){
    char buffer[1024];
    while(true){
        int len = read(inotifyFd, buffer, sizeof(buffer));
        if(len < 0){
            Alert a;
            a.d = Danger::CRITICAL;
            a.message = "Errore: read inotify fallita";
            a.timestamp = time(nullptr);
            queue.push(a);
            break;
        }
        struct inotify_event* event = (struct inotify_event*)buffer;
        if(event->mask & IN_MODIFY){
            readNewLines();
        }
    }
}
