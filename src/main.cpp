#include <iostream>

#include "command_registry.hpp"
#include "parser.hpp"

int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // REPL 
  while (true) {
    std::cout << "$ ";

    std::string input;
    if (!std::getline(std::cin, input)) {
      break;
    }

    const Command command { parse_command(input) };
    if (command.empty) {
      continue;
    }

    if (command.raw == "exit") {
      break;
    }

    const auto handler { get_handler(command.raw) };
    if (!handler) {
      std::cout << command.raw << ": command not found\n";
      continue;
    }

    handler(command);
  }

  return 0;
}
