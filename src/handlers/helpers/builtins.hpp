#pragma once

#include <array>
#include <string_view>

constexpr std::array<std::string_view, 5> builtins{
  "exit",
  "echo",
  "type",
  "jobs",
  "history"
};