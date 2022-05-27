#include <regex>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <sysexits.h>

#include "grep_factory.hpp"
#include "glob.hpp"
#include "parse_input.hpp"

void help() {
   std::cout <<
R"~(
[cgrep]
-------
Conditionally grep files using multiple
required(+), rejected(-) and matched(=) patterns.
   
[usage]
-------
cgrep [-iEfh] [-f=patterns] [file ...]
   
[options]
---------
-i / --ignore-case
   Sets grep to ignore case when matching.
-E / --extended-regexp
   Sets pattern matching to egrep.
-f= / --file=
   Reads patterns from a file.
-h / --help
   Shows this page.
)~";
}

void print(const std::vector<std::string>& lines, const std::string& filename) {
   for(const auto& line : lines) {
      std::cout << filename << ": ";
      std::cout << line << '\n';
   }
}

bool has_all(const std::vector<bool>& required) {
   return std::all_of(required.begin(), required.end(), [](bool flag){ return flag; });
}

void grep(std::vector<std::string> filenames, GrepFactory::Patterns patterns) {
   std::string buffer;
   std::vector<std::string> line_buffer;
   bool found_match = false;
   const auto is_match = [&buffer](const std::regex& grep){
      return std::regex_search(buffer, grep);
   };
   
   for(const auto& filename : filenames) {
      std::ifstream file(filename);
      std::vector<bool> required_matches(patterns.require.size());
      bool rejected = false;
      
      while(std::getline(file, buffer)) {
         // Check rejected patterns
         if(std::any_of(patterns.reject.begin(), patterns.reject.end(), is_match)) {
            rejected = true;
            break;
         }
         
         // Check required patterns
         for(int i = 0; i < required_matches.size(); ++i) {
            if(required_matches[i]) continue;
            else required_matches[i] = is_match(patterns.require[i]);
         }
         
         // Check match patterns
         if(std::any_of(patterns.match.begin(), patterns.match.end(), is_match)) {
            line_buffer.push_back(std::move(buffer));
         }
      }
      
      if(not rejected and has_all(required_matches)) {
         // This is where the different output strategies should go.
         found_match = true;
         
         print(line_buffer, filename);
      }
      line_buffer.clear();
   }
   
   if(not found_match) exit(EXIT_FAILURE);
}

int main(int argc, const char * argv[]) {
   const auto options = get_options(argc, argv);
   if(argc == 1 or options.count("-h") or options.count("--help")) {
      help();
      exit(EXIT_SUCCESS);
   }
   
   const auto filepaths = glob_files(get_args(argc, argv));
   if(filepaths.empty()) {
      std::cout << "No files to search\n";
      exit(EXIT_FAILURE);
   }
   
   const auto patterns = GrepFactory(options).get_patterns();
   if(patterns.match.empty()) {
      std::cout << "No patterns to match (as denoted by the '=')\n";
      exit(EX_USAGE);
   }

   grep(filepaths, patterns);
}
