#pragma once

#include <array>
#include <string_view>

constexpr std::array<std::string_view, 4> builtins{
  "exit",
  "echo",
  "type",
  "jobs"
};