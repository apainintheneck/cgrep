#pragma once

#include <string>
#include <vector>
#include <map>

namespace parse {

std::vector<std::string> args(const int argc, const char* argv[]);

std::map<std::string, std::string> options(const int argc, const char* argv[]);

std::string outfile_path(const std::map<std::string, std::string>& options);

} // namespace parse
