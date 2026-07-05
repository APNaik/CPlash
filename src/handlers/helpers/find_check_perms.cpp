#include "find_check_perms.hpp"

std::optional<std::string> find_and_check_permissions(const std::vector<std::string> &directories, const std::string &command_name) {
  for (const std::string &dir : directories) {
    fs::path search_path { fs::path(dir) / command_name };
    std::error_code ec {};

    if (fs::exists(search_path, ec) && fs::is_regular_file(search_path, ec)) {
      fs::perms p { fs::status(search_path, ec).permissions() };
      if (!ec && (p & (fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec)) != fs::perms::none) {
        return search_path.string();
      }
    }
  }

  return std::nullopt;
}