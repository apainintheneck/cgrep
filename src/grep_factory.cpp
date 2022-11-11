#include "grep_factory.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sysexits.h>

#include "util.hpp"

const auto prev_query_path = util::expand_path("~/.cgrep_previous_query");

GrepFactory::GrepFactory(const std::map<std::string, std::string>& options) {
   // Build regex flags
   flags_ = std::regex::optimize;
   
   if(options.count("-i") or options.count("--ignore-case")) {
      flags_ |= std::regex::icase;
   }
   
   if(options.count("-E") or options.count("--extended-regexp")) {
      flags_ |= std::regex::egrep;
   } else {
      flags_ |= std::regex::grep;
   }
   
   // Get pattern file if applicable
   if((options.count("--pattern-file") || options.count("-p"))
      && (options.count("--again") || options.count("-a"))) {
      
      std::cerr << "Error: Conflicting options -a and -p\n";
      exit(EX_USAGE);
   }
   
   if(options.count("--pattern-file")) filepath_ = util::expand_path(options.at("--pattern-file"));
   else if(options.count("-p")) filepath_ = util::expand_path(options.at("-p"));
   else if(options.count("--again") || options.count("-a")) filepath_ = prev_query_path;
}

// Parse query pattern by either trimming whitespace or
// only returning the string between quotes or backticks.
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
   // Open file to save current query
   std::ofstream save_query_file(prev_query_path);
   if(not save_query_file.is_open()) {
      std::cerr << "Error: Unable to save query\n\n";
   }
   
   // Prompt user for query
   std::cerr << "# Enter grep patterns starting with...\n";
   std::cerr << "#   '+' for required patterns\n";
   std::cerr << "#   '-' for rejected patterns\n";
   std::cerr << "#   '=' for matched patterns\n";
   std::cerr << "# Enter a blank line to submit.\n";
   
   GrepFactory::Patterns patterns;
   
   std::cerr << "> ";
   std::string buffer;
   // Parse each pattern
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
            save_query_file << "+ " << pattern << '\n';
            break;
         case '-':
            patterns.reject.push_back(grep);
            save_query_file << "- " << pattern << '\n';
            break;
         case '=':
            patterns.match.push_back(grep);
            save_query_file << "= " << pattern << '\n';
            break;
         default:
            std::cerr << "Invalid option (" << buffer.front() << ")\n";
            std::cerr << "Use one of ['+', '-', '=']\n";
            break;
      }
      std::cerr << "> ";
   }
   
   std::cerr << '\n';
   
   return patterns;
}

// From file
GrepFactory::Patterns GrepFactory::get_patterns_from_file() {
   std::ifstream file(filepath_);
   if(not file.is_open()) {
      std::cerr << "Unable to open pattern file: " << filepath_ << '\n';
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
   
   // Save current query for later
   if(filepath_ != prev_query_path) {
      std::error_code err;
      
      if(not std::filesystem::copy_file(filepath_, prev_query_path, err)) {
         std::cerr << "Error: Unable to save query\n\n";
      }
   }
   
   return patterns;
}

// Decide between getting patterns from input or a file.
GrepFactory::Patterns GrepFactory::get_patterns() {
   return filepath_.empty()
         ? get_patterns_from_stdin()
         : get_patterns_from_file();
}
