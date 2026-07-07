#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>

#include "command_registry.hpp"
#include "parser.hpp"

namespace {

class StdoutRedirect {
public:
  explicit StdoutRedirect(const std::string& path)
    : original_stdout_fd { dup(STDOUT_FILENO) },
      redirected_fd { open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644) } {
    if (original_stdout_fd == -1) {
      std::perror("dup");
      return;
    }

    if (redirected_fd == -1) {
      std::perror(path.c_str());
      return;
    }

    if (dup2(redirected_fd, STDOUT_FILENO) == -1) {
      std::perror("dup2");
      return;
    }

    active = true;
  }

  ~StdoutRedirect() {
    std::cout.flush();

    if (active && original_stdout_fd != -1) {
      if (dup2(original_stdout_fd, STDOUT_FILENO) == -1) {
        std::perror("dup2");
      }
    }

    if (redirected_fd != -1) {
      close(redirected_fd);
    }

    if (original_stdout_fd != -1) {
      close(original_stdout_fd);
    }
  }

  bool ok() const {
    return active;
  }

private:
  int original_stdout_fd { -1 };
  int redirected_fd { -1 };
  bool active { false };
};

}  // namespace

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

    if (command.stdout_redirect_path.has_value()) {
      StdoutRedirect redirect { command.stdout_redirect_path.value() };
      if (!redirect.ok()) {
        continue;
      }

      handler(command);
      continue;
    }

    handler(command);
  }

  return 0;
}
