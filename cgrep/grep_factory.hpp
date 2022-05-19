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
   using pattern = std::pair<char, std::string>;
   
   void set_options(const std::set<std::string>& options);
   lists build_regexes(const std::vector<pattern>& patterns);
private:
   // Options
   std::regex_constants::syntax_option_type flags;
};

std::vector<GrepFactory::pattern> read_patterns();
