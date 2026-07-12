#include "completion_handler.hpp"
#include "helpers/extract_PATH_dirs.hpp"

#include <set>
#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;
std::vector<std::string> system_executables;

char* generator(const char* text, int state){
  static size_t list_index;
  std::string partial(text);

  // first TAB keystroke, initialize the trackers
  if(!state){
    list_index = 0;
  }

  while(list_index < builtins.size()){
    const std::string command { builtins[list_index] };
    list_index++;

    if(command.starts_with(partial)){
      // Readline expects a dynamically allocated c string
      return strdup(command.c_str());
    }
  }

  // Subtracting builtins.size() normalizes our index mapping back to 0 for the second array
  while(list_index - builtins.size() < system_executables.size()) {
    const std::string& command = system_executables[list_index - builtins.size()];
    list_index++;

    if (command.starts_with(partial)) {
      return strdup(command.c_str());
    }
  }

  // No matches exist
  return nullptr;
}

char** completion(const char* text, int start, int end){
  // rl_attempted_completion_over = 1;    // Prevents readline from falling back to file completion
  
  return rl_completion_matches(text, generator);
}

void init_executables_registry(){
  std::set<std::string> unique_execs;
  std::vector<std::string> exec_paths { extract_directories() };

  for(const std::string &dir_path: exec_paths){
    std::error_code ec {};
    if (!fs::exists(dir_path, ec) || !fs::is_directory(dir_path, ec)) {
      continue;
    }

    for(const auto &entry: fs::directory_iterator(dir_path, ec)){
      if(ec) continue;

      if (entry.is_regular_file(ec)) {
        fs::perms p = entry.status(ec).permissions();
        bool is_executable = (p & (fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec)) != fs::perms::none;
        if (!ec && is_executable) {
          unique_execs.insert(entry.path().filename().string());
        }
      }
    }
  }

  system_executables.assign(unique_execs.begin(), unique_execs.end());
}
