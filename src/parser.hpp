#pragma once

#include "command.hpp"
#include "pipeline.hpp"

#include <variant>

std::variant<Command, Pipeline> parse_composite_command(const std::string& input);