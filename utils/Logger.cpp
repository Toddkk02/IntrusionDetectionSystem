#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include "Logger.hpp"


std::string Logger::GetTimestamp(){
    time_t timestamp;
    time(&timestamp);
    std::string ts = ctime(&timestamp);
    ts.pop_back();
    return ts;
}

Logger::Logger(std::string filename){
    logFile.open(filename);
}

Logger::~Logger(){
    logFile.close();
}

std::string Logger::getLevelString(Danger d){
    switch(d){
        case Danger::INFO:     return "INFO";
        case Danger::WARNING:  return "WARNING";
        case Danger::ALERT:    return "ALERT";
        case Danger::CRITICAL: return "CRITICAL";
        default:               return "UNKNOWN";
    }
}

std::string Logger::getColor(Danger d){
    switch(d){
        case Danger::INFO:     return Color::WHITE;
        case Danger::WARNING:  return Color::YELLOW;
        case Danger::ALERT:    return Color::RED;
        case Danger::CRITICAL: return Color::MAGENTA;
        default:               return Color::RESET;
    }
}

void Logger::log(Danger level, std::string message){
    std::string timestamp = GetTimestamp();
    std::string color = getColor(level);
    std::string levelString = getLevelString(level);
    std::cout << color << "[" << timestamp << "] [" << levelString << "] " << message << Color::RESET << std::endl;
    logFile << "[" << timestamp << "] [" << levelString << "] " << message << std::endl;
}

