#pragma once

#include <iostream>

#include "../command.hpp"

inline void handle_echo(const Command& command) {
  for (std::size_t index = 0; index < command.arguments.size(); ++index) {
    if (index > 0) {
      std::cout << ' ';
    }

    std::cout << command.arguments[index];
  }

  std::cout << '\n';
}

