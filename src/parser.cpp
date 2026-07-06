#include "parser.hpp"

#include <iostream>

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

    std::string argument;

    // To handle single and double quoted arguments
    bool inside_single_quote { false };
    bool inside_double_quote { false };

    while (argument_start < input.size()) {
      const char current { input[argument_start] };

      if(current == '\\' && argument_start + 1 < input.size()){
        argument.push_back(input[++argument_start]);
        ++argument_start;
        continue;
      }

      if (current == '\'' && !inside_double_quote) {
        inside_single_quote = !inside_single_quote;
        ++argument_start;
        continue;
      }

      if (current == '"' && !inside_single_quote) {
        inside_double_quote = !inside_double_quote;
        ++argument_start;
        continue;
      }

      if (current == ' ' && !inside_single_quote && !inside_double_quote) {
        break;
      }

      argument.push_back(current);
      ++argument_start;
    }

    if (inside_single_quote || inside_double_quote) {
      std::cerr << "Quotes not closed properly" << std::endl;
      command.empty = true;
      return command;
    }

    command.arguments.push_back(argument);
    argument_start = find_first_space(input, argument_start);
  }

  return command;
}
