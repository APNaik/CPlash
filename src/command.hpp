#pragma once

#include <vector>
#include <string>

struct Command {
  std::string raw;
  std::vector<std::string> arguments;
  bool empty { false } ;
};