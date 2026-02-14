#include "PortScanDetector.hpp"

PortScanDetector::PortScanDetector(AlertQueue& q) : queue(q) {}

void PortScanDetector::analyze(std::string srcIp, int dstPort){
    time_t now = time(nullptr);

    if(scanMap.find(srcIp) == scanMap.end()){
        scanMap[srcIp].firstSeen = now;
    }

    scanMap[srcIp].ports.insert(dstPort);

    int elapsed = (int)(now - scanMap[srcIp].firstSeen);
    int portCount = (int)scanMap[srcIp].ports.size();

    if(elapsed > timeWindow){
        scanMap[srcIp].ports.clear();
        scanMap[srcIp].firstSeen = now;
        alreadyAlerted.erase(srcIp);
        return;
    }

    if(portCount >= threshold && alreadyAlerted.find(srcIp) == alreadyAlerted.end()){
        alreadyAlerted.insert(srcIp);
        Alert a;
        a.d = Danger::CRITICAL;
        a.message = "Port scan rilevato da " + srcIp +
                    " — " + std::to_string(portCount) +
                    " porte in " + std::to_string(elapsed) + "s";
        a.timestamp = now;
        queue.push(a);
    }
}