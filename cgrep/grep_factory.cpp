#include "grep_factory.hpp"

#include <iostream>
#include <fstream>
#include <sysexits.h>

void GrepFactory::set_options(const std::map<std::string, std::string>& options) {
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

std::string parse_pattern(const std::string& line) {
   const auto start = line.find_first_not_of(' ', 1);
   
   if(start == std::string::npos) return "";
   
   if(line[start] == '\"' or line[start] == '\'' or line[start] == '`') {
      const char quote = line[start];
      const auto end = line.find_last_of(quote);
      
      if(end == start) return "";
      else return line.substr(start + 1, end - start - 1);
   }
   
   return line.substr(start, line.find_last_not_of(' ') - start + 1);
}

// From stdin
GrepFactory::lists GrepFactory::get_regexes() {
   std::cout << "# Enter grep patterns starting with...\n";
   std::cout << "#   '+' for required patterns\n";
   std::cout << "#   '-' for rejected patterns\n";
   std::cout << "#   '=' for matched patterns\n";
   std::cout << "# Enter a blank line to submit.\n";
   
   GrepFactory::lists regexes;
   
   std::cout << "> ";
   std::string buffer;
   while(std::getline(std::cin, buffer)) {
      if(buffer.empty()) break;
      
      // For now just have the first char be the token.
      // Then the pattern.
      auto pattern = parse_pattern(buffer);
      if(pattern.empty()) continue;
      
      auto regex = std::regex(pattern, flags);
      switch(buffer.front()) {
         case '+':
            regexes.require.push_back(regex);
            break;
         case '-':
            regexes.reject.push_back(regex);
            break;
         case '=':
            regexes.match.push_back(regex);
            break;
         default:
            std::cout << "Invalid option (" << buffer.front() << ")\n";
            std::cout << "Use one of ['+', '-', '=']\n";
            break;
      }
      std::cout << "> ";
   }
   
   std::cout << '\n';
   
   return regexes;
}

// From file
GrepFactory::lists GrepFactory::get_regexes(const std::string& filepath) {
   std::ifstream file(filepath);
   if(not file.is_open()) {
      std::cout << "Unable to open pattern file: " << filepath << '\n';
      exit(EX_NOINPUT);
   }
   
   GrepFactory::lists regexes;
   
   std::string buffer;
   while(std::getline(file, buffer)) {
      if(buffer.empty()) break;
      
      // For now just have the first char be the token.
      // Then the pattern.
      auto pattern = parse_pattern(buffer);
      if(pattern.empty()) continue;
      
      auto regex = std::regex(pattern, flags);
      switch(buffer.front()) {
         case '+':
            regexes.require.push_back(regex);
            break;
         case '-':
            regexes.reject.push_back(regex);
            break;
         case '=':
            regexes.match.push_back(regex);
            break;
         default:
            // Skip everything else
            break;
      }
   }
   
   return regexes;
}

// Decide between getting patterns from input or a file.
GrepFactory::lists GrepFactory::get_regexes(const std::map<std::string, std::string>& options) {
   if(options.count("-f")) return get_regexes(options.at("-f"));
   if(options.count("--file")) return get_regexes(options.at("--file"));
   
   return get_regexes();
}
