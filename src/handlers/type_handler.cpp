#include "type_handler.hpp"
#include "helpers/extract_PATH_dirs.hpp"
#include "helpers/find_check_perms.hpp"
#include "helpers/builtins.hpp"

#include <algorithm>
#include <iostream>

namespace {

bool is_builtin_command(std::string_view command_name) {
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

  std::vector<std::string> directories { extract_directories() };
  if(directories.size() == 0){
    std::cerr << "Error: PATH variable not found\n";
    return;
  }

  auto search_path { find_and_check_permissions(directories, command_name) };
  if(search_path.has_value()){
    std::cout << command_name << " is " << search_path.value() << "\n";
  }
  else{
    std::cout << command_name << ": not found\n"; 
  }
}
