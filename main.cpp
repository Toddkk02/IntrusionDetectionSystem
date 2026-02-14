#include <iostream>
#include <map>
#include <vector>
#include <ctime>
#include <thread>
#include <atomic>
#include "utils/Logger.hpp"
#include "hids/BruteForceDetector.hpp"
#include "hids/LogWatcher.hpp"
#include "hids/ProcessMonitor.hpp"
#include "nids/PacketSniffer.hpp"
#include "core/AlertQueue.hpp"
#include "core/Config.hpp"
#include "ui/Dashboard.hpp"
#include "ui/TuiDashboard.hpp"

std::atomic<bool> t2_alive{true};
std::atomic<bool> t3_alive{true};
std::atomic<bool> t4_alive{true};

void logWatcherThread(AlertQueue& queue){
    t2_alive = true;
    try {
        LogWatcher watcher(Config::AUTH_LOG, queue);
        Alert a;
        a.d = Danger::INFO;
        a.message = "LogWatcher attivo su /var/log/auth.log";
        a.timestamp = time(nullptr);
        queue.push(a);
        watcher.watch();
    } catch(...) {}
    t2_alive = false;
}

void processMonitorThread(AlertQueue& queue){
    t3_alive = true;
    try {
        ProcessMonitor monitor(queue);
        while(true){
            monitor.monitor();
            std::this_thread::sleep_for(std::chrono::seconds(Config::PROCESS_SCAN_INTERVAL));
        }
    } catch(...) {}
    t3_alive = false;
}

void packetSnifferThread(AlertQueue& queue){
    t4_alive = true;
    try {
        PacketSniffer sniffer(queue);
        sniffer.sniff();
    } catch(...) {}
    t4_alive = false;
}

void watchdogThread(AlertQueue& queue){
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(10));

        if(!t2_alive){
            Alert a;
            a.d = Danger::CRITICAL;
            a.message = "WATCHDOG: LogWatcher morto - riavvio in corso";
            a.timestamp = time(nullptr);
            queue.push(a);
            t2_alive = true;
            std::thread t(logWatcherThread, std::ref(queue));
            t.detach();
        }
        if(!t3_alive){
            Alert a;
            a.d = Danger::CRITICAL;
            a.message = "WATCHDOG: ProcessMonitor morto - riavvio in corso";
            a.timestamp = time(nullptr);
            queue.push(a);
            t3_alive = true;
            std::thread t(processMonitorThread, std::ref(queue));
            t.detach();
        }
        if(!t4_alive){
            Alert a;
            a.d = Danger::CRITICAL;
            a.message = "WATCHDOG: PacketSniffer morto - riavvio in corso";
            a.timestamp = time(nullptr);
            queue.push(a);
            t4_alive = true;
            std::thread t(packetSnifferThread, std::ref(queue));
            t.detach();
        }
    }
}

int main(int argc, char* argv[]){
    bool headless = false;
    bool tui = false;

    for(int i = 1; i < argc; i++){
        if(std::string(argv[i]) == "--headless") headless = true;
        if(std::string(argv[i]) == "--tui")      tui = true;
    }

    Logger logger(Config::LOG_FILE);
    logger.log(Danger::INFO, "IDS avviato");

    AlertQueue queue;
    Alert start;
    start.d = Danger::INFO;
    start.message = "IDS avviato - monitoraggio attivo";
    start.timestamp = time(nullptr);
    queue.push(start);

    std::thread t2(logWatcherThread, std::ref(queue));
    std::thread t3(processMonitorThread, std::ref(queue));
    std::thread t4(packetSnifferThread, std::ref(queue));
    std::thread t5(watchdogThread, std::ref(queue));
    t2.detach();
    t3.detach();
    t4.detach();
    t5.detach();

    if(headless){
        logger.log(Danger::INFO, "Modalita headless - solo log su file");
        while(true){
            Alert a;
            while(queue.pop(a))
                logger.log(a.d, a.message);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    } else if(tui){
        TuiDashboard tuidash(queue);
        tuidash.init();
        tuidash.run(queue);
    } else {
        Dashboard dashboard(queue);
        dashboard.init();
        dashboard.run();
    }

    return 0;
}