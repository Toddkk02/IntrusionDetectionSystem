#pragma once
#include <string>
#include <map>
#include <vector>
#include <set>
#include <ctime>
#include "../core/AlertQueue.hpp"
#include "../core/Config.hpp"
struct ScanData {
    std::set<int> ports;
    time_t firstSeen;
};

class PortScanDetector {
private:
    std::map<std::string, ScanData> scanMap;
    AlertQueue& queue;
    std::set<std::string> alreadyAlerted;
    int threshold  = Config::PORT_SCAN_THRESHOLD;
    int timeWindow = Config::PORT_SCAN_WINDOW;
public:
    PortScanDetector(AlertQueue& queue);
    void analyze(std::string srcIp, int dstPort);
};