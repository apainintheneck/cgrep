#pragma once

#include <string>
#include <vector>
#include <map>

std::vector<std::string> parse_args(const int argc, const char* argv[]);

std::map<std::string, std::string> parse_options(const int argc, const char* argv[]);

std::string parse_out_file_path(const std::map<std::string, std::string>& options);
