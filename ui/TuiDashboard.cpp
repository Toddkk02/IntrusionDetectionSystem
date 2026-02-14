#include "TuiDashboard.hpp"
#include <algorithm>

// colori ncurses
#define COLOR_PAIR_INFO     1
#define COLOR_PAIR_WARNING  2
#define COLOR_PAIR_ALERT    3
#define COLOR_PAIR_CRITICAL 4
#define COLOR_PAIR_HEADER   5
#define COLOR_PAIR_BORDER   6

TuiDashboard::TuiDashboard(AlertQueue& queue){
    (void)queue;
    startTime = time(nullptr);
    running = true;
}

TuiDashboard::~TuiDashboard(){
    delwin(headerWin);
    delwin(statsWin);
    delwin(alertWin);
    endwin();
}

void TuiDashboard::initColors(){
    start_color();
    init_pair(COLOR_PAIR_INFO,     COLOR_CYAN,    COLOR_BLACK);
    init_pair(COLOR_PAIR_WARNING,  COLOR_YELLOW,  COLOR_BLACK);
    init_pair(COLOR_PAIR_ALERT,    COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_PAIR_CRITICAL, COLOR_RED,     COLOR_BLACK);
    init_pair(COLOR_PAIR_HEADER,   COLOR_GREEN,   COLOR_BLACK);
    init_pair(COLOR_PAIR_BORDER,   COLOR_WHITE,   COLOR_BLACK);
}

void TuiDashboard::init(){
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(500);  // refresh ogni 500ms
    initColors();

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // finestre
    headerWin = newwin(3,  cols, 0,  0);
    statsWin  = newwin(4,  cols, 3,  0);
    alertWin  = newwin(rows - 7, cols, 7, 0);
    scrollok(alertWin, TRUE);
}

void TuiDashboard::drawBorder(WINDOW* win){
    wattron(win, COLOR_PAIR(COLOR_PAIR_BORDER));
    box(win, 0, 0);
    wattroff(win, COLOR_PAIR(COLOR_PAIR_BORDER));
}

void TuiDashboard::drawHeader(){
    werase(headerWin);
    drawBorder(headerWin);
    wattron(headerWin, COLOR_PAIR(COLOR_PAIR_HEADER) | A_BOLD);
    mvwprintw(headerWin, 1, 2, "IDS - Intrusion Detection System");
    wattroff(headerWin, COLOR_PAIR(COLOR_PAIR_HEADER) | A_BOLD);
    wattron(headerWin, COLOR_PAIR(COLOR_PAIR_BORDER));
    mvwprintw(headerWin, 1, 38, "| Status: RUNNING | Press 'q' to quit");
    wattroff(headerWin, COLOR_PAIR(COLOR_PAIR_BORDER));
    wrefresh(headerWin);
}

void TuiDashboard::drawStats(){
    werase(statsWin);
    drawBorder(statsWin);

    time_t now = time(nullptr);
    int uptime  = (int)(now - startTime);
    int hours   = uptime / 3600;
    int minutes = (uptime % 3600) / 60;
    int seconds = uptime % 60;

    int countInfo = 0, countWarning = 0, countAlert = 0, countCritical = 0;
    for(auto& a : alertLog){
        switch(a.d){
            case Danger::INFO:     countInfo++;     break;
            case Danger::WARNING:  countWarning++;  break;
            case Danger::ALERT:    countAlert++;    break;
            case Danger::CRITICAL: countCritical++; break;
        }
    }

    mvwprintw(statsWin, 1, 2, "Uptime: %02d:%02d:%02d", hours, minutes, seconds);
    mvwprintw(statsWin, 1, 25, "Totale alert: %d", (int)alertLog.size());

    wattron(statsWin, COLOR_PAIR(COLOR_PAIR_INFO));
    mvwprintw(statsWin, 2, 2, "INFO: %d", countInfo);
    wattroff(statsWin, COLOR_PAIR(COLOR_PAIR_INFO));

    wattron(statsWin, COLOR_PAIR(COLOR_PAIR_WARNING));
    mvwprintw(statsWin, 2, 15, "WARNING: %d", countWarning);
    wattroff(statsWin, COLOR_PAIR(COLOR_PAIR_WARNING));

    wattron(statsWin, COLOR_PAIR(COLOR_PAIR_ALERT));
    mvwprintw(statsWin, 2, 30, "ALERT: %d", countAlert);
    wattroff(statsWin, COLOR_PAIR(COLOR_PAIR_ALERT));

    wattron(statsWin, COLOR_PAIR(COLOR_PAIR_CRITICAL) | A_BOLD);
    mvwprintw(statsWin, 2, 45, "CRITICAL: %d", countCritical);
    wattroff(statsWin, COLOR_PAIR(COLOR_PAIR_CRITICAL) | A_BOLD);

    wrefresh(statsWin);
}

void TuiDashboard::drawAlerts(){
    werase(alertWin);
    drawBorder(alertWin);

    int rows, cols;
    getmaxyx(alertWin, rows, cols);
    (void)rows;

    // header colonne
    wattron(alertWin, A_BOLD | A_UNDERLINE);
    mvwprintw(alertWin, 1, 2,  "%-20s", "Timestamp");
    mvwprintw(alertWin, 1, 23, "%-10s", "Severity");
    mvwprintw(alertWin, 1, 34, "%-s",   "Message");
    wattroff(alertWin, A_BOLD | A_UNDERLINE);

    // mostra ultimi alert che entrano nella finestra
    int maxRows = rows - 3;
    int start = (int)alertLog.size() > maxRows ? (int)alertLog.size() - maxRows : 0;

    int row = 2;
    for(int i = start; i < (int)alertLog.size(); i++){
        auto& alert = alertLog[i];

        char timebuf[32];
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&alert.timestamp));

        int colorPair;
        const char* sevStr;
        switch(alert.d){
            case Danger::INFO:     colorPair = COLOR_PAIR_INFO;     sevStr = "INFO";     break;
            case Danger::WARNING:  colorPair = COLOR_PAIR_WARNING;  sevStr = "WARNING";  break;
            case Danger::ALERT:    colorPair = COLOR_PAIR_ALERT;    sevStr = "ALERT";    break;
            case Danger::CRITICAL: colorPair = COLOR_PAIR_CRITICAL; sevStr = "CRITICAL"; break;
            default:               colorPair = COLOR_PAIR_INFO;     sevStr = "INFO";     break;
        }

        mvwprintw(alertWin, row, 2, "%-20s", timebuf);

        wattron(alertWin, COLOR_PAIR(colorPair) | (alert.d == Danger::CRITICAL ? A_BOLD : 0));
        mvwprintw(alertWin, row, 23, "%-10s", sevStr);
        wattroff(alertWin, COLOR_PAIR(colorPair) | A_BOLD);

        // tronca messaggio se troppo lungo
        std::string msg = alert.message;
        if((int)msg.size() > cols - 36)
            msg = msg.substr(0, cols - 39) + "...";
        mvwprintw(alertWin, row, 34, "%s", msg.c_str());

        row++;
    }

    wrefresh(alertWin);
}

void TuiDashboard::run(AlertQueue& queue){
    while(running){
        // consuma alert
        Alert a;
        while(queue.pop(a)){
            alertLog.push_back(a);
            if(alertLog.size() > Config::MAX_ALERT_LOG)
                alertLog.pop_front();
        }

        drawHeader();
        drawStats();
        drawAlerts();

        // input non bloccante
        int ch = getch();
        if(ch == 'q' || ch == 'Q')
            running = false;
    }
}