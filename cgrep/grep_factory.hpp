#pragma once

#include <vector>
#include <regex>
#include <string>
#include <utility>
#include <set>

class GrepFactory {
public:
   struct lists {
      std::vector<std::regex> require;
      std::vector<std::regex> reject;
      std::vector<std::regex> match;
   };
   
   void set_options(const std::set<std::string>& options);
   lists get_regexes();
private:
   // Options
   std::regex_constants::syntax_option_type flags;
};
