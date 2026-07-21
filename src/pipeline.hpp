#pragma once

#include "command.hpp"

struct Pipeline {
  std::vector<Command> commands;
  
  bool empty() const { 
    return commands.empty() || commands[0].empty; 
  }
};