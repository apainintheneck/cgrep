#pragma once

#include <string>
#include <vector>
#include <map>

std::vector<std::string> get_args(const int argc, const char* argv[]);

std::map<std::string, std::string> get_options(const int argc, const char* argv[]);
