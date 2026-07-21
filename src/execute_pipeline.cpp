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
  const size_t num_cmds {pipeline.commands.size() };
  if(num_cmds < 2) return;
  
  std::vector<pid_t> pids;
  int prev_fd = STDIN_FILENO;
  
  for(size_t i {0}; i < num_cmds; ++i){
    int curr_pipe[2] = {-1, -1}; // 0 -> read end and 1 -> write end

    if(i < num_cmds - 1){
      if(pipe(curr_pipe) == -1){
        std::perror("pipe");
        if(prev_fd != STDIN_FILENO) close(prev_fd);
        return;
      }
    }
    pid_t pid { fork() };
    if(pid == -1){
      std::perror("fork");
      if(prev_fd != STDIN_FILENO) close(prev_fd);
      if(curr_pipe[0] != -1) close(curr_pipe[0]);
      if(curr_pipe[1] != -1) close(curr_pipe[1]);
      break;
    }
    if(pid == 0){
      int fdin { (i == 0) ? STDIN_FILENO : prev_fd };
      int fdout { (i == num_cmds - 1) ? STDOUT_FILENO : curr_pipe[1] };
      // new child does not read from the pipe
      if(curr_pipe[0] != -1) close(curr_pipe[0]);
      run_pipeline_stage(pipeline.commands[i], fdin, fdout);
    }

    pids.push_back(pid);
    if(prev_fd != STDIN_FILENO){
      close(prev_fd);
    }
    if(i < num_cmds - 1){
      close(curr_pipe[1]);  // Crucial: close write-end in parent so reader receives EOF
      prev_fd = curr_pipe[0];  // Save read-end for command i + 1
    }
  } 
  for (pid_t pid : pids) {
    int status;
    waitpid(pid, &status, 0);
  }
}