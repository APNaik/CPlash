#pragma once

#include <string>
#include <vector>

struct Command {
  std::string raw;
  std::vector<std::string> arguments;
  bool empty { false } ;
};

Command parse_command(const std::string& input);