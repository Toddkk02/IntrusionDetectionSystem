#pragma once
#include <string>
#include <map>
#include <vector>
#include <ctime>
#include "../core/AlertQueue.hpp"  // ← aggiungila

void ReadLogFile(std::string path, std::map<std::string, std::vector<time_t>>& attempts, AlertQueue& queue);
bool alert(std::string ip, std::map<std::string, std::vector<time_t>>& attempts, AlertQueue& queue);