#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>

#include "handlers/completion_handler.hpp"
#include "command_registry.hpp"
#include "parser.hpp"

namespace {

// RAII implementation
class FileRedirect {
public:
  explicit FileRedirect(const std::string& path, const int target_fd, Mode MODE)
    : _original_fd { dup(target_fd) },
      _target_fd { target_fd } {
    int flags = O_WRONLY | O_CREAT;
    if(MODE == Mode::APPEND){
      flags |= O_APPEND;
    }
    else{
      flags |= O_TRUNC;
    }

    _redirected_fd = open(path.c_str(), flags, 0644);

    if (_original_fd == -1) {
      std::perror("dup");
      return;
    }

    if (_redirected_fd == -1) {
      std::perror(path.c_str());
      return;
    }

    if (dup2(_redirected_fd, target_fd) == -1) {
      std::perror("dup2");
      return;
    }

    active = true;
  }

  ~FileRedirect() {
    if(_target_fd == STDOUT_FILENO){
      std::cout.flush();
    }
    else if(_target_fd == STDERR_FILENO){
      std::cerr.flush();
    }

    if(active && _original_fd != -1){
      if(dup2(_original_fd, _target_fd) == -1){
        std::perror("dup2 restore");
      }
    }

    if(_original_fd != -1) close(_original_fd);
    if(_redirected_fd != -1) close(_redirected_fd);
  } 

  bool ok() const {
    return active;
  }

private:
  int _original_fd { -1 };
  int _redirected_fd { -1 };
  int _target_fd { -1 };
  bool active { false };
};

}  // namespace

int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  rl_attempted_completion_function = completion;
  rl_bind_key('\t', rl_complete);
  
  // REPL 
  while (true) {
    char *raw_input = readline("$ ");

    if(!raw_input){
      std::cout << "\n";
      break;
    }

    std::string input(raw_input);
    
    free(raw_input);
    if(input.empty()){
      continue;
    }

    add_history(input.c_str());

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
    
    std::optional<FileRedirect> _stdout;
    std::optional<FileRedirect> _stderr;

    // We have > or 1> token
    if(command.stdout_redirect_path.has_value()){
      _stdout.emplace(command.stdout_redirect_path.value(), STDOUT_FILENO, command.stdout_mode);
      if(!_stdout->ok()) continue;
    }
    
    // We have 2> token
    if(command.stderr_redirect_path.has_value()){
      _stderr.emplace(command.stderr_redirect_path.value(), STDERR_FILENO, command.stderr_mode);
      if(!_stderr->ok()) continue;
    }

    handler(command);
  }

  return 0;
}
