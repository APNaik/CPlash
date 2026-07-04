#include "parser.hpp"

namespace {
std::size_t find_first_non_space(const std::string& text) {
  return text.find_first_not_of(' ');
}

std::size_t find_first_space(const std::string& text, std::size_t start) {
  return text.find(' ', start);
}

}  // empty namespace keeps the functions private to the current file

Command parse_command(const std::string& input) {
  Command command;

  const std::size_t first_non_space { find_first_non_space(input) };
  if (first_non_space == std::string::npos) {
    command.empty = true;
    return command;
  }

  const std::size_t first_space { find_first_space(input, first_non_space) };
  command.raw = input.substr(first_non_space, first_space - first_non_space);

  std::size_t argument_start { first_space };
  while (argument_start != std::string::npos) {
    argument_start = input.find_first_not_of(' ', argument_start);
    if (argument_start == std::string::npos) {
      break;
    }

    const std::size_t argument_end { find_first_space(input, argument_start) };
    command.arguments.push_back(input.substr(argument_start, argument_end - argument_start));
    argument_start = argument_end;
  }

  return command;
}