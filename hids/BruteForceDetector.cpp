#include <fstream>
#include <string>
#include <regex>
#include <iostream>
#include <ctime>
#include <map>
#include "BruteForceDetector.hpp" 
#include "../core/AlertQueue.hpp"

bool alert(std::string ip, std::map<std::string, std::vector<time_t>>& attempts, AlertQueue& queue){
    int count = 0;   
    time_t now = time(nullptr);
    for(time_t t : attempts[ip]){
        if (now - t <= 60)
            count++;
    }
    if(count >= 5){
        Alert a;
        a.d = Danger::CRITICAL;
        a.message = "Brute force da " + ip;
        a.timestamp = time(nullptr);
        queue.push(a);
        return true;
    }
    return false;
}

void ReadLogFile(std::string path, std::map<std::string, std::vector<time_t>>& attempts, AlertQueue& queue){
    std::regex pattern("Failed password.*from (\\d+\\.\\d+\\.\\d+\\.\\d+)");
    std::ifstream file(path); 
    if (!file.is_open()) return;
    std::string str;
    while (std::getline(file, str)){ 
        std::smatch match;
        if (std::regex_search(str, match, pattern)){
            std::string ip = match[1];
            attempts[ip].push_back(time(nullptr));
            alert(ip, attempts, queue);
        }
    }
}