#pragma once

namespace Config {

    // BruteForce
    constexpr int BRUTE_FORCE_THRESHOLD = 5;      // tentativi prima dell'alert
    constexpr int BRUTE_FORCE_WINDOW    = 60;     // secondi

    // PortScan
    constexpr int PORT_SCAN_THRESHOLD   = 15;     // porte uniche
    constexpr int PORT_SCAN_WINDOW      = 10;     // secondi

    // ProcessMonitor
    constexpr int PROCESS_SCAN_INTERVAL = 5;      // secondi tra scansioni

    // Dashboard
    constexpr int MAX_ALERT_LOG         = 1000;   // max righe in memoria

    // Log
    constexpr const char* LOG_FILE      = "ids.log";
    constexpr const char* AUTH_LOG      = "/var/log/auth.log";
    constexpr const char* PROC_PATH     = "/proc";

    // Porte sensibili
    constexpr int SENSITIVE_PORTS[]     = {22, 23, 3306, 5432, 6379, 27017};
    constexpr int SENSITIVE_PORTS_COUNT = 6;
}