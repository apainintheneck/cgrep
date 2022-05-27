#include "grep_factory.hpp"

#include <iostream>
#include <fstream>
#include <sysexits.h>

GrepFactory::GrepFactory(const std::map<std::string, std::string>& options) {
   flags_ = std::regex::optimize;
   
   if(options.count("-i") or options.count("--ignore-case")) {
      flags_ |= std::regex::icase;
   }
   
   if(options.count("-E") or options.count("--extended-regexp")) {
      flags_ |= std::regex::egrep;
   } else {
      flags_ |= std::regex::grep;
   }
   
   if(options.count("--file")) filepath_ = options.at("--file");
   else if(options.count("-f")) filepath_ = options.at("-f");
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
GrepFactory::Patterns GrepFactory::get_patterns_from_stdin() {
   std::cout << "# Enter grep patterns starting with...\n";
   std::cout << "#   '+' for required patterns\n";
   std::cout << "#   '-' for rejected patterns\n";
   std::cout << "#   '=' for matched patterns\n";
   std::cout << "# Enter a blank line to submit.\n";
   
   GrepFactory::Patterns patterns;
   
   std::cout << "> ";
   std::string buffer;
   while(std::getline(std::cin, buffer)) {
      if(buffer.empty()) break;
      
      // For now just have the first char be the token.
      // Then the pattern.
      auto pattern = parse_pattern(buffer);
      if(pattern.empty()) continue;
      
      auto grep = std::regex(pattern, flags_);
      switch(buffer.front()) {
         case '+':
            patterns.require.push_back(grep);
            break;
         case '-':
            patterns.reject.push_back(grep);
            break;
         case '=':
            patterns.match.push_back(grep);
            break;
         default:
            std::cout << "Invalid option (" << buffer.front() << ")\n";
            std::cout << "Use one of ['+', '-', '=']\n";
            break;
      }
      std::cout << "> ";
   }
   
   std::cout << '\n';
   
   return patterns;
}

// From file
GrepFactory::Patterns GrepFactory::get_patterns_from_file() {
   std::ifstream file(filepath_);
   if(not file.is_open()) {
      std::cout << "Unable to open pattern file: " << filepath_ << '\n';
      exit(EX_NOINPUT);
   }
   
   GrepFactory::Patterns patterns;
   
   std::string buffer;
   while(std::getline(file, buffer)) {
      if(buffer.empty()) break;
      
      // For now just have the first char be the token.
      // Then the pattern.
      auto pattern = parse_pattern(buffer);
      if(pattern.empty()) continue;
      
      auto grep = std::regex(pattern, flags_);
      switch(buffer.front()) {
         case '+':
            patterns.require.push_back(grep);
            break;
         case '-':
            patterns.reject.push_back(grep);
            break;
         case '=':
            patterns.match.push_back(grep);
            break;
         default:
            // Skip everything else
            break;
      }
   }
   
   return patterns;
}

// Decide between getting patterns from input or a file.
GrepFactory::Patterns GrepFactory::get_patterns() {
   if(filepath_.empty()) {
      return get_patterns_from_stdin();
   } else {
      return get_patterns_from_file();
   }
}
