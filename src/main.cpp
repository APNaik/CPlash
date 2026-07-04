#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // REPL
  while(1){
    std::cout << "$ ";
    std::string command;
    std::getline(std::cin, command);
    if(command == "exit"){
      break;
    }
    else if(command.substr(0, 5) == "echo "){
      std::cout << command.substr(5) << "\n";
    }
    else if(command.substr(0, 5) == "type "){
      std::string next_command = command.substr(5);
      if(next_command == "exit" || next_command == "type" || next_command == "echo"){
        std::cout << next_command << " is a shell builtin\n";
      }
      else{
        std::cout << next_command << ": command not found" << std::endl;
      }
    }
    else std::cout << command << ": command not found" << std::endl;
  }
  
  return 0;
}
