#ifndef UTIL_HPP
#define UTIL_HPP

#include "config.hpp"

void printConfig(const std::vector<ServerConfig>& servers);
std::map<int, std::string> initStatus();
std::string getCurrentTime();

#endif // UTIL_HPP
