#pragma once
#include <string>
#include <fstream>

namespace Color{
    constexpr const char* RESET   = "\033[0m";
    constexpr const char* WHITE   = "\033[37m";
    constexpr const char* YELLOW  = "\033[33m";
    constexpr const char* RED     = "\033[31m";
    constexpr const char* MAGENTA = "\033[35m";
}

enum class Danger { INFO, WARNING, ALERT, CRITICAL };

class Logger {
private:
    std::ofstream logFile;
    std::string GetTimestamp();
    std::string getLevelString(Danger d);
    std::string getColor(Danger d);
public:
    Logger(std::string filename);
    ~Logger();
    void log(Danger level, std::string message);
};