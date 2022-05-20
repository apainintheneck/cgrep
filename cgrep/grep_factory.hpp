#pragma once

#include <vector>
#include <regex>
#include <string>
#include <map>

class GrepFactory {
public:
   struct lists {
      std::vector<std::regex> require;
      std::vector<std::regex> reject;
      std::vector<std::regex> match;
   };
   
   void set_options(const std::map<std::string, std::string>& options);
   // From stdin
   lists get_regexes();
   // From file
   lists get_regexes(const std::string& filepath);
   // Decide on which to call based on options
   lists get_regexes(const std::map<std::string, std::string>& options);
private:
   // Options
   std::regex_constants::syntax_option_type flags;
};
