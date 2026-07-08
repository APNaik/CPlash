#include "completion_handler.hpp"

char* generator(const char* text, int state){
  static size_t len, list_index;
  std::string partial(text);

  // first TAB keystroke, initialize the trackers
  if(!state){
    len = partial.length();
    list_index = 0;
  }

  while(list_index < builtins.size()){
    const std::string command { builtins[list_index] };
    list_index++;

    if(command.rfind(partial, 0) == 0){
      // Readline expects a dynamically allocated c string
      return strdup(command.c_str());
    }
  }
  // No matches exist
  return nullptr;
}

char** completion(const char* text, int start, int end){
  rl_attempted_completion_over = 1;    // Prevents readline from falling back to file completion
  
  return rl_completion_matches(text, generator);
}