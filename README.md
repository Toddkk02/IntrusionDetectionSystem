# Intrusion Detection System (IDS)

A lightweight, multi-threaded Intrusion Detection System written in C++ for Linux.
Monitors network traffic, system logs, and running processes in real time,
with three interface modes: GUI (Dear ImGui), TUI (ncurses), and headless (daemon).

---

## Features

- **HIDS** - Host-based detection
  - Brute force SSH detection via `/var/log/auth.log`
  - Real-time log monitoring with inotify
  - Suspicious process detection via `/proc`

- **NIDS** - Network-based detection
  - Raw socket packet sniffing
  - Port scan detection (SYN flood analysis)
  - Sensitive port connection alerts

- **Three interface modes**
  - `--gui` - Graphical dashboard (Dear ImGui + SDL2)
  - `--tui` - Terminal UI (ncurses)
  - `--headless` - Daemon mode for servers (systemd compatible)

- **Reliability**
  - Thread watchdog - auto-restarts crashed modules
  - Configurable thresholds via `core/Config.hpp`
  - CSV export of alert log

---

## Dependencies
```bash
sudo apt install libsdl2-dev libncurses-dev g++ make
```

ImGui is included as a subdirectory — no separate installation needed.

---

## Build
```bash
git clone https://github.com/toddkk02/IntrusionDetectionSystem.git
cd IntrusionDetectionSystem
make re
```

---

## Usage
```bash
# Graphical mode (requires display)
sudo ./ids

# Terminal UI mode
sudo ./ids --tui

# Headless daemon mode
sudo ./ids --headless
```

Root privileges are required for raw socket access and log reading.

---

## Install as system service
```bash
sudo make install
```

This copies the binary to `/usr/local/bin/ids` and installs a systemd service
that starts automatically on boot.
```bash
# Service management
sudo systemctl start ids
sudo systemctl stop ids
sudo systemctl status ids
sudo journalctl -u ids -f
```

To uninstall:
```bash
sudo make uninstall
```

---

## Configuration

All thresholds and paths are configurable in `core/Config.hpp`:
```cpp
namespace Config {
    constexpr int BRUTE_FORCE_THRESHOLD = 5;    // failed attempts before alert
    constexpr int BRUTE_FORCE_WINDOW    = 60;   // seconds
    constexpr int PORT_SCAN_THRESHOLD   = 15;   // unique ports
    constexpr int PORT_SCAN_WINDOW      = 10;   // seconds
    constexpr int PROCESS_SCAN_INTERVAL = 5;    // seconds between scans
    constexpr int MAX_ALERT_LOG         = 1000; // max alerts in memory
}
```

---

## Architecture
```
main.cpp
├── hids/
│   ├── LogWatcher       - inotify-based real-time log monitoring
│   ├── BruteForceDetector - SSH brute force detection
│   └── ProcessMonitor   - /proc scanning for suspicious processes
├── nids/
│   ├── PacketSniffer    - raw socket TCP/UDP capture
│   └── PortScanDetector - SYN-based port scan detection
├── core/
│   ├── AlertQueue       - thread-safe alert queue
│   └── Config           - global configuration
├── ui/
│   ├── Dashboard        - ImGui graphical interface
│   └── TuiDashboard     - ncurses terminal interface
└── utils/
    └── Logger           - ANSI colored console + file logging
```

---

## Alert severity levels

| Level    | Color   | Description                        |
|----------|---------|------------------------------------|
| INFO     | Blue    | Informational events               |
| WARNING  | Yellow  | Suspicious but not critical        |
| ALERT    | Orange  | Confirmed suspicious activity      |
| CRITICAL | Red     | Active attack or intrusion attempt |

---

## Testing
```bash
# Simulate SSH brute force
sudo bash -c 'for i in {1..10}; do
    echo "Feb 14 10:23:$i server sshd[1234]: Failed password for root from 192.168.1.5 port 22 ssh2" >> /var/log/auth.log
done'

# Simulate port scan
sudo nmap -sS 127.0.0.1

# Simulate suspicious process
ncat -l 4444
```

---

## License

MIT License - see LICENSE file for details.

---

## Author

Alessandro - [GitHub](https://github.com/TUONOME)

