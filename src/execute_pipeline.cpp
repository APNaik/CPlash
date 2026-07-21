#include "execute_pipeline.hpp"
#include "command_registry.hpp"
#include "handlers/helpers/builtins.hpp"

#include <iostream>
#include <unistd.h>
#include <vector>
#include <fcntl.h>
#include <sys/wait.h>

namespace {
bool is_builtin_command(std::string_view command_name) {
	return std::find(builtins.begin(), builtins.end(), command_name) != builtins.end();
}

void setup_stage_redirection(const Command& command){
  if(command.stdout_redirect_path.has_value()){
    int flags { O_WRONLY | O_CREAT | (command.stdout_mode == Mode::APPEND ? O_APPEND : O_TRUNC) };
    int fd { open(command.stdout_redirect_path->c_str(), flags, 0644)};
    if(fd == -1){
      std::perror(command.stdout_redirect_path->c_str());
      _exit(1);
    } 
    dup2(fd, STDOUT_FILENO);
    close(fd);
  }

  if (command.stderr_redirect_path.has_value()) {
    int flags = O_WRONLY | O_CREAT | (command.stderr_mode == Mode::APPEND ? O_APPEND : O_TRUNC);
    int fd = open(command.stderr_redirect_path->c_str(), flags, 0644);
    if (fd == -1) {
      std::perror(command.stderr_redirect_path->c_str());
      _exit(1);
    }
    dup2(fd, STDERR_FILENO);
    close(fd);
  }

}

void run_pipeline_stage(const Command& command, int fdin, int fdout){
  // 1. Redirect stdin if needed
  if (fdin != STDIN_FILENO) {
    if (dup2(fdin, STDIN_FILENO) == -1) {
      std::perror("dup2 stdin");
      _exit(1);
    }
    close(fdin);
  }

  // 2. Redirect stdout if needed
  if (fdout != STDOUT_FILENO) {
    if (dup2(fdout, STDOUT_FILENO) == -1) {
      std::perror("dup2 stdout");
      _exit(1);
    }
    close(fdout);
  }

  setup_stage_redirection(command);

  if (is_builtin_command(command.raw)) {
    const auto handler = get_handler(command.raw);
    handler(command);
    _exit(0);
  }
  std::vector<char*> argv;
  argv.push_back(const_cast<char*>(command.raw.c_str()));
  for (const auto& arg : command.arguments) {
    argv.push_back(const_cast<char*>(arg.c_str()));
  }
  argv.push_back(nullptr);

  execvp(argv[0], argv.data());

  // If execvp returns, execution failed (e.g., binary not found)
  std::cerr << command.raw << ": command not found\n";
  _exit(1);
}

} // namespace

void execute_pipeline(const Pipeline& pipeline){
  if(pipeline.commands.size() < 2) return;

  int pipefd[2];   // 0 -> read end and 1 -> write end
  if(pipe(pipefd) == -1){
    std::perror("pipe");
    return;
  }
  const auto& left_command { pipeline.commands[0] };
  const auto& right_command { pipeline.commands[1] };
  pid_t pid1 = fork();
  if (pid1 == -1) {
    std::perror("fork");
    close(pipefd[0]);
    close(pipefd[1]);
    return;
  }
  if(pid1 == 0){
    // Child 1 does not read from the pipe
    close(pipefd[0]);
    run_pipeline_stage(left_command, STDIN_FILENO, pipefd[1]);
  }

  pid_t pid2 = fork();
  if(pid2 == -1) {
    std::perror("fork");
    close(pipefd[0]);
    close(pipefd[1]);
    int status;
    waitpid(pid1, &status, 0);
    return;
  }
  if(pid2 == 0){
    // Child 2 does not write to pipe
    close(pipefd[1]);
    run_pipeline_stage(right_command, pipefd[0], STDOUT_FILENO);
  }

  // Close all the pipe descriptors for parent process
  close(pipefd[0]);
  close(pipefd[1]);

  int status;
  waitpid(pid1, &status, 0);
  waitpid(pid2, &status, 0);
}