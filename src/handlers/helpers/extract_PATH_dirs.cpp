#include "extract_PATH_dirs.hpp"

#include <sstream>

std::vector<std::string> extract_directories() {
  const char *rawPATH { std::getenv("PATH") };
  if (rawPATH == nullptr) {
    return {};
  }

  std::string path_string(rawPATH);
  std::vector<std::string> directories {};
  std::stringstream ss(path_string);
  std::string directory {};

  while (std::getline(ss, directory, ':')) {
    if (!directory.empty()) {
      directories.push_back(directory);
    }
  }

  return directories;
}