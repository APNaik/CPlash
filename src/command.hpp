#pragma once

#include <optional>
#include <vector>
#include <string>

struct Command {
  std::string raw;
  std::vector<std::string> arguments;
  std::optional<std::string> stdout_redirect_path;
  bool empty { false } ;
};
