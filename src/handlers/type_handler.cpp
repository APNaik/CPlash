#include "type_handler.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <sstream>
#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

std::vector<std::string> builtins {
	{"exit", "echo", "type"}
};

namespace {

bool is_builtin_command(const std::string& command_name) {
	return std::find(builtins.begin(), builtins.end(), command_name) != builtins.end();
}

}  // empty namespace keeps the functions private to the current file

void handle_type(const Command& command) {
	if (command.arguments.empty()) {
		return;
	}

	const std::string& command_name { command.arguments.front() };
	if (is_builtin_command(command_name)) {
		std::cout << command_name << " is a shell builtin\n";
    return;
	}

	const char *rawPATH { std::getenv("PATH") };
  if(rawPATH == nullptr){
    std::cerr << "Error: PATH variable not found\n";
    return;
  }
  std::string path_string(rawPATH);
  // normal PATH string looks something like:
  // /usr/local/bin:/usr/bin:/bin:/usr/games in POSIX. We should split it at :
  std::vector<std::string> directories {};
  std::stringstream ss(path_string);
  std::string directory {};
  while(std::getline(ss, directory, ':')){
    // Skip empty entries like ::
    if(!directory.empty()){
      directories.push_back(directory);
    }
  }

  bool found { false };
  for(const std::string &dir: directories){
    fs::path search_path { fs::path(dir) / command_name };
    std::error_code ec {};
    if(fs::exists(search_path, ec) && fs::is_regular_file(search_path, ec)){
      // Check the file permissions
      fs::perms p { fs::status(search_path, ec).permissions() };
      if(!ec && (p & (fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec)) != fs::perms::none){
        std::cout << command_name << " is " << search_path.string() << "\n";
        return;
      }
    }
  }
  std::cout << command_name << ": not found\n";
}
