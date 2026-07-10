#pragma once

#include <optional>
#include <vector>
#include <string>

enum class Mode {
  TRUNC,
  APPEND
};

struct Command {
  std::string raw;
  std::vector<std::string> arguments;

  // Output redirection essentials
  std::optional<std::string> stdout_redirect_path;
  std::optional<std::string> stderr_redirect_path;
  Mode stdout_mode { Mode::TRUNC };
  Mode stderr_mode { Mode::TRUNC };
  
  bool empty { false } ;
};
