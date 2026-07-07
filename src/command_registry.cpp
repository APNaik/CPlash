#include "command_registry.hpp"
#include "handlers/echo_handler.hpp"
#include "handlers/execution_handler.hpp"
#include "handlers/type_handler.hpp"

Handler get_handler(const std::string& command_name) {
  if (command_name == "echo") {
    return handle_echo;
  }

  if (command_name == "type") {
    return handle_type;
  }

  return handle_execution;
}
