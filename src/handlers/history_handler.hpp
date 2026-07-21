#pragma once

#include "../command.hpp"

#include <vector>
#include <string>
#include <readline/history.h>

extern std::vector<std::string> history;
void handle_history(const Command& command);