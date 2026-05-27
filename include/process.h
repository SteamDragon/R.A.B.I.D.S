#pragma once
#include <string>
#include <vector>

std::string exec(const char* cmd);
int createChild(std::vector<std::string> arguments);
