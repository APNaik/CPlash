#include "parser.hpp"

#include <iostream>

namespace {
std::size_t find_first_non_space(const std::string& text) {
  return text.find_first_not_of(' ');
}

bool is_redirect_at(const std::string& input, std::size_t idx) {
  if(input[idx] == '>') return true;

  if((input[idx] == '1' || input[idx] == '2') && idx + 1 < input.length() && input[idx+1] == '>') return true;

  return false;
}

bool parse_token(const std::string &input, std::size_t &idx, std::string &token, bool* token_was_quoted = nullptr) {
  bool inside_single_quote { false };
  bool inside_double_quote { false };
  bool was_quoted { false };

  while (idx < input.size()) {
    const char current { input[idx] };

    if (!inside_single_quote && !inside_double_quote && is_redirect_at(input, idx)) {
      if (!token.empty()) {
        break;
      }

      if(input[idx] == '>'){
        if(input[idx+1] == '>'){
          token = ">>";
        }
        else {
          token = ">";
        }
      }
      else if(idx + 2 < input.length() && input[idx+2] == '>'){
        if(input[idx] == '1'){
          token = "1>>";
        }
        else{
          token = "2>>";
        }
      }
      else{
        if(input[idx] == '1'){
          token = "1>";
        }
        else{
          token = "2>";
        }
      }
      idx += token.size();
      return true;
    }

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
      was_quoted = true;
      ++idx;
      continue;
    }

    if (current == '"' && !inside_single_quote) {
      inside_double_quote = !inside_double_quote;
      was_quoted = true;
      ++idx;
      continue;
    }

    if (current == ' ' && !inside_single_quote && !inside_double_quote) {
      break;
    }

    token.push_back(current);
    ++idx;
  }

  if (token_was_quoted != nullptr) {
    *token_was_quoted = was_quoted;
  }

  return !inside_single_quote && !inside_double_quote;
}

bool is_redirect_token(const std::string& token) {
  return token == ">" || token == "1>" || token == "2>" ||
         token == ">>" || token == "1>>" || token == "2>>";
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

    bool argument_was_quoted { false };
    if (!parse_token(input, argument_start, argument, &argument_was_quoted)) {
      std::cerr << "Quotes not closed properly" << std::endl;
      command.empty = true;
      return command;
    }

    if (!argument_was_quoted && is_redirect_token(argument)) {
      std::string redirect_opt { argument };  // Can be >, 1>, 2>, >>, 1>> or 2>>
      argument_start = input.find_first_not_of(' ', argument_start);
      if (argument_start == std::string::npos) {
        std::cerr << "Redirection target missing" << std::endl;
        command.empty = true;
        return command;
      }

      std::string redirect_path;
      bool redirect_path_was_quoted { false };
      if (!parse_token(input, argument_start, redirect_path, &redirect_path_was_quoted)) {
        std::cerr << "Quotes not closed properly" << std::endl;
        command.empty = true;
        return command;
      }

      // Redirection token cannot be followed by another unquoted redirection
      // token immediately.
      if (!redirect_path_was_quoted && is_redirect_token(redirect_path)) {
        std::cerr << "Redirection target missing" << std::endl;
        command.empty = true;
        return command;
      }

      if (redirect_opt == ">" || redirect_opt == "1>" || redirect_opt == ">>" || redirect_opt == "1>>") {
        // Check if it's an append operation
        if (redirect_opt == ">>" || redirect_opt == "1>>") {
            command.stdout_mode = Mode::APPEND;
        } else {
            command.stdout_mode = Mode::TRUNC;
        }
        command.stdout_redirect_path = redirect_path;
      } 
      else if (redirect_opt == "2>" || redirect_opt == "2>>") {
        // Check if it's an append operation
        if (redirect_opt == "2>>") {
          command.stderr_mode = Mode::APPEND;
        } else {
          command.stderr_mode = Mode::TRUNC;
        }
        command.stderr_redirect_path = redirect_path;
      }
      
      continue;
    }

    command.arguments.push_back(argument);
  }

  // Recognizing background jobs with "&"
  if(!command.arguments.empty() && command.arguments.back() == "&"){
    command.run_in_background = true;
    command.arguments.pop_back();
  }
  return command;
}
