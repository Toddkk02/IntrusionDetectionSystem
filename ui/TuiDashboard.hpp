#pragma once
#include <ncurses.h>
#include <deque>
#include <ctime>
#include "../core/AlertQueue.hpp"
#include "../core/Config.hpp"

class TuiDashboard {
private:
    WINDOW* headerWin;
    WINDOW* statsWin;
    WINDOW* alertWin;
    std::deque<Alert> alertLog;
    time_t startTime;
    bool running;

    void initColors();
    void drawHeader();
    void drawStats();
    void drawAlerts();
    void drawBorder(WINDOW* win);

public:
    TuiDashboard(AlertQueue& queue);
    ~TuiDashboard();
    void init();
    void run(AlertQueue& queue);
};