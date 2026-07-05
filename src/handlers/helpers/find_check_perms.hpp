#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include <system_error>

namespace fs = std::filesystem;

std::optional<std::string> find_and_check_permissions(const std::vector<std::string> &directories, const std::string &command_name);
