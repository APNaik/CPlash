#pragma once

#include <vector>
#include <string>
#include <readline/readline.h>
#include <readline/history.h>

#include "helpers/builtins.hpp"

char* generator(const char* text, int state);
char** completion(const char* text, int start, int end); 