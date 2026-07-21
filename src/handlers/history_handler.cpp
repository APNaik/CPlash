#include "history_handler.hpp"

#include <iostream>

std::vector<std::string> history;

void handle_history(const Command& command){
  HIST_ENTRY** list { ::history_list() };
  if(!list){
    return;
  }

  for(int i {0}; list[i] != nullptr; ++i){
    if(list[i]->line != nullptr){
      history.emplace_back(list[i]->line);
    }
  }

  size_t n { history.size() };
  if(command.arguments.size() != 0){
    n = std::stoi(command.arguments[0]);
  }
  for(size_t i { history.size() - n }; i < history.size(); ++i){
    std::cout << "\t" << i+1 << "  " << history[i] << "\n";
  }
  history.clear();
}