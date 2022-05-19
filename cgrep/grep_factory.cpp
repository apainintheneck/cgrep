#include "grep_factory.hpp"

#include <iostream>

void GrepFactory::set_options(const std::set<std::string>& options) {
   flags = std::regex::optimize;
   
   if(options.count("-i") or options.count("--ignore-case")) {
      flags |= std::regex::icase;
   }
   
   if(options.count("-E") or options.count("--extended-regexp")) {
      flags |= std::regex::egrep;
   } else {
      flags |= std::regex::grep;
   }
}

GrepFactory::lists GrepFactory::build_regexes(const std::vector<pattern> &patterns) {
   GrepFactory::lists regexes;

   for(const auto& pattern : patterns) {
      switch(pattern.first) {
         case '+':
            regexes.require.push_back(
               std::regex(pattern.second, flags)
            );
            break;
         case '-':
            regexes.reject.push_back(
               std::regex(pattern.second, flags)
            );
            break;
         case '=':
            regexes.match.push_back(
               std::regex(pattern.second, flags)
            );
            break;
      }
   }
   
   return regexes;
}

// Modify this to allow reading patterns from a file instead of just standard out.
std::vector<GrepFactory::pattern> read_patterns() {
   std::cout << "Enter grep patterns starting with...\n";
   std::cout << "  '+' is for required patterns\n";
   std::cout << "  '-' is for rejected patterns\n";
   std::cout << "  '=' is for matched patterns\n";
   std::cout << "Enter a blank line to submit.\n";
   
   std::vector<GrepFactory::pattern> patterns;
   
   std::cout << "> ";
   std::string buffer;
   while(std::getline(std::cin, buffer)) {
      if(buffer.empty()) break;
      
      switch(buffer.front()) {
         case '+':
         case '-':
         case '=':
            // For now just have the first char be the token.
            // Then a space.
            // Then the pattern.
            patterns.push_back({ buffer.front(), buffer.substr(2) });
            break;
         default:
            std::cout << "Invalid option (" << buffer.front() << ")\n";
            std::cout << "Use one of ['+', '-', '=']\n";
      }
      std::cout << "> ";
   }
   
   std::cout << '\n';
   
   return patterns;
}
