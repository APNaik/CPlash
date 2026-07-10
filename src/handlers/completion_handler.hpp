/*
This is the only handler that is referenced and used directly by main.cpp
Intended to perform low-level shell manipulation using readline library.
Performs builtin command completion when <TAB> is hit.
*/

#pragma once

#include <vector>
#include <string>
#include <readline/readline.h>
#include <readline/history.h>

#include "helpers/builtins.hpp"

extern std::vector<std::string> system_executables;
void init_executables_registry();

char* generator(const char* text, int state);
char** completion(const char* text, int start, int end); 