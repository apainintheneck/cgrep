#pragma once

#include <vector>
#include <string>

namespace util {

std::vector<std::string> glob_files(const std::vector<std::string>& patterns);
std::string expand_path(const std::string& path);

} // namespace util
