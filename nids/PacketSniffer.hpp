#pragma once
#include <string>
#include <vector>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../core/AlertQueue.hpp"
#include "PortScanDetector.hpp"

class PacketSniffer {
private:
    int sockFd;
    AlertQueue& queue;
    PortScanDetector portScanDetector;

    void analyzePacket(unsigned char* buffer, int size);
    void analyzeTCP(struct iphdr* ip, struct tcphdr* tcp);
    void analyzeUDP(struct iphdr* ip, struct udphdr* udp);
public:
    PacketSniffer(AlertQueue& queue);
    ~PacketSniffer();
    void sniff();
};