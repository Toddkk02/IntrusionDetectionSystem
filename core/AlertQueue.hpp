#pragma once
#include <queue>
#include <mutex>
#include <string>
#include "../utils/Logger.hpp"
struct Alert {
    Danger d;
    std::string message;
    time_t timestamp;

};
class AlertQueue {
    private:
    std::queue<Alert> queue;
    std::mutex mtx;
    public:
    void push(Alert alert);
    bool pop(Alert& alert);
    bool empty();
};