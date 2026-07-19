#include "helpers/extract_PATH_dirs.hpp"
#include "helpers/find_check_perms.hpp"
#include "helpers/builtins.hpp"
#include "helpers/jobs.hpp"
#include "executables_handler.hpp"
#include "../command.hpp"

#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

namespace {

bool is_builtin_command(std::string_view command_name) {
	return std::find(builtins.begin(), builtins.end(), command_name) != builtins.end();
}

}  // empty namespace keeps the functions private to the current file

void handle_executables(const Command& command) {
  if (command.empty) {
    return;
  }

  const std::string& command_name = command.raw;

  if (is_builtin_command(command_name)) {
    return;
  }

  auto directories = extract_directories();
  auto search_path = find_and_check_permissions(directories, command_name);

  if (!search_path) {
    std::cout << command_name << ": command not found\n";
    return;
  }

  pid_t pid = fork();
  if (pid == 0) {
    // process is in background so detach stdin from the terminal
    // this prevents, stealing inputs meant for parent process and causing seg-faults. 
    if(command.run_in_background){
      int dev_null = open("/dev/null", O_RDONLY);
      if(dev_null >= 0){
        dup2(dev_null, STDIN_FILENO);
        close(dev_null);
      }
    }
    std::vector<char*> argv;
    argv.reserve(command.arguments.size() + 2);
    argv.push_back(const_cast<char*>(command_name.c_str()));
    for (const auto& arg : command.arguments) {
      argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);

    execv(search_path->c_str(), argv.data());
    std::perror("execv");
    _exit(1);
  }

  if(command.run_in_background){
    int job_num { jim.acquire_id() };
    if (job_num == -1) {
      std::cerr << "Error: Too many background jobs!\n";
      return;
    }
    
    std::string cmd { command.raw };
    for(auto &arg: command.arguments){
      cmd += " " + arg;
    }
    cmd += " &";

    std::cout << "[" << job_num << "] " << pid << "\n";
    active_background_jobs.push_back({
      job_num,
      pid,
      cmd,
      "Running"
    });
  }
  else{
    int status { 0 };
    waitpid(pid, &status, 0);
  }
}
