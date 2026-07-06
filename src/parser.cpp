#include "parser.hpp"

#include <iostream>

namespace {
std::size_t find_first_non_space(const std::string& text) {
  return text.find_first_not_of(' ');
}

std::size_t find_first_space(const std::string& text, std::size_t start) {
  return text.find(' ', start);
}

bool parse_token(const std::string &input, std::size_t &idx, std::string &token) {
  bool inside_single_quote { false };
  bool inside_double_quote { false };

  while (idx < input.size()) {
    const char current { input[idx] };

    if (current == '\\' && idx + 1 < input.size()) {
      const char next { input[idx + 1] };

      if (!inside_double_quote && !inside_single_quote) {
        token.push_back(input[++idx]);
        ++idx;
        continue;
      }

      if (inside_double_quote && (next == '"' || next == '\\')) {
        token.push_back(input[++idx]);
        ++idx;
        continue;
      }
    }

    if (current == '\'' && !inside_double_quote) {
      inside_single_quote = !inside_single_quote;
      ++idx;
      continue;
    }

    if (current == '"' && !inside_single_quote) {
      inside_double_quote = !inside_double_quote;
      ++idx;
      continue;
    }

    if (current == ' ' && !inside_single_quote && !inside_double_quote) {
      break;
    }

    token.push_back(current);
    ++idx;
  }

  return !inside_single_quote && !inside_double_quote;
}

}  // empty namespace keeps the functions private to the current file

Command parse_command(const std::string& input) {
  Command command;

  const std::size_t first_non_space { find_first_non_space(input) };
  if (first_non_space == std::string::npos) {
    command.empty = true;
    return command;
  }

  std::size_t argument_start { first_non_space };
  if (!parse_token(input, argument_start, command.raw)) {
    std::cerr << "Quotes not closed properly" << std::endl;
    command.empty = true;
    return command;
  }

  while (argument_start != std::string::npos) {
    argument_start = input.find_first_not_of(' ', argument_start);
    if (argument_start == std::string::npos) {
      break;
    }

    std::string argument;

    if (!parse_token(input, argument_start, argument)) {
      std::cerr << "Quotes not closed properly" << std::endl;
      command.empty = true;
      return command;
    }

    command.arguments.push_back(argument);
  }

  return command;
}
