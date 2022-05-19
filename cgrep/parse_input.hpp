#pragma once

#include <string>
#include <vector>
#include <set>

std::vector<std::string> get_args(const int argc, const char* argv[]);

std::set<std::string> get_options(const int argc, const char* argv[]);
