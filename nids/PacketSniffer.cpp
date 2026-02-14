#include "PacketSniffer.hpp"
#include <unistd.h>
#include <cstring>
#include <ctime>
#include "../core/Config.hpp"
PacketSniffer::PacketSniffer(AlertQueue& q) 
    : queue(q), portScanDetector(q) {
    sockFd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if(sockFd < 0){
        Alert a;
        a.d = Danger::CRITICAL;
        a.message = "PacketSniffer: impossibile aprire raw socket (serve root)";
        a.timestamp = time(nullptr);
        queue.push(a);
    }
}

PacketSniffer::~PacketSniffer(){
    close(sockFd);
}

void PacketSniffer::analyzeTCP(struct iphdr* ip, struct tcphdr* tcp){
    char srcIp[INET_ADDRSTRLEN];
    char dstIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip->saddr, srcIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &ip->daddr, dstIp, INET_ADDRSTRLEN);

    int srcPort = ntohs(tcp->source);
    int dstPort = ntohs(tcp->dest);

    // SYN senza ACK -> passa al PortScanDetector
    if(tcp->syn && !tcp->ack){
        portScanDetector.analyze(std::string(srcIp), dstPort);
    }

    // porte sensibili -> alert diretto
    std::vector<int> sensitivePorts = {22, 23, 3306, 5432, 6379, 27017};
    for(int i = 0; i < Config::SENSITIVE_PORTS_COUNT; i++){
        if(dstPort == Config::SENSITIVE_PORTS[i]){
            Alert a;
            a.d = Danger::ALERT;
            a.message = "Connessione a porta sensibile: " + std::string(srcIp) +
                    " -> " + std::string(dstIp) + ":" + std::to_string(dstPort);
            a.timestamp = time(nullptr);
            queue.push(a);
            break;
        }
    }
}

void PacketSniffer::analyzeUDP(struct iphdr* ip, struct udphdr* udp){
    // per ora non implementato
    (void)ip;
    (void)udp;
}

void PacketSniffer::analyzePacket(unsigned char* buffer, int size){
    struct iphdr* ip = (struct iphdr*)(buffer);

    if(size < (int)sizeof(struct iphdr)) return;

    switch(ip->protocol){
        case IPPROTO_TCP: {
            struct tcphdr* tcp = (struct tcphdr*)(buffer + ip->ihl * 4);
            analyzeTCP(ip, tcp);
            break;
        }
        case IPPROTO_UDP: {
            struct udphdr* udp = (struct udphdr*)(buffer + ip->ihl * 4);
            analyzeUDP(ip, udp);
            break;
        }
    }
}

void PacketSniffer::sniff(){
    unsigned char buffer[65536];

    Alert a;
    a.d = Danger::INFO;
    a.message = "PacketSniffer avviato - monitoraggio rete attivo";
    a.timestamp = time(nullptr);
    queue.push(a);

    while(true){
        int size = recv(sockFd, buffer, sizeof(buffer), 0);
        if(size > 0)
            analyzePacket(buffer, size);
    }
}