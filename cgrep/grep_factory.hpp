#pragma once

#include <vector>
#include <regex>
#include <string>
#include <map>

class GrepFactory {
public:
   struct Patterns {
      std::vector<std::regex> require;
      std::vector<std::regex> reject;
      std::vector<std::regex> match;
   };
   
   GrepFactory(const std::map<std::string, std::string>& options);
   // Decide on which to call based on options
   Patterns get_patterns();
   
private:
   Patterns get_patterns_from_stdin();
   Patterns get_patterns_from_file();
   
   // Options
   std::regex_constants::syntax_option_type flags;
   // Filepath
   std::string filepath;
};
