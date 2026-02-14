#include "AlertQueue.hpp"


void AlertQueue::push(Alert alert){
    std::lock_guard<std::mutex> lock(mtx);
    queue.push(alert);
}

bool AlertQueue::pop(Alert& alert){
    std::lock_guard<std::mutex> lock(mtx);
    if(queue.empty()) return false;
    alert = queue.front();
    queue.pop();
    return true;
}

bool AlertQueue::empty(){
    std::lock_guard<std::mutex> lock(mtx);
    return queue.empty();
}