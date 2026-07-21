/*
This is a router to each handler. It returns a callable object that handles the execution of 
matching command.
The handler returned by the router is wrapped in a polymorphic function wrapper inside main.cpp
*/

#include "command_registry.hpp"
#include "handlers/echo_handler.hpp"
#include "handlers/executables_handler.hpp"
#include "handlers/type_handler.hpp"
#include "handlers/jobs_handler.hpp"
#include "handlers/history_handler.hpp"

Handler get_handler(const std::string& command_name) {
  if (command_name == "echo") {
    return handle_echo;
  }

  if (command_name == "type") {
    return handle_type;
  }

  if(command_name == "jobs"){
    return handle_jobs;
  }

  if(command_name == "history"){
    return handle_history;
  }
  
  return handle_executables;
}
