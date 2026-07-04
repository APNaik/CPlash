#pragma once

#include <functional>
#include <string>

#include "parser.hpp"

using Handler = std::function<void(const Command&)>;

Handler get_handler(const std::string& command_name);