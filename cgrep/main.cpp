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

void print(const std::vector<std::string>& lines) {
   for(const auto& line : lines) {
      std::cout << line << '\n';
   }
}

void print(const std::vector<std::string>& lines, const std::string& filename) {
   for(const auto& line : lines) {
      std::cout << filename << ": ";
      std::cout << line << '\n';
   }
}

bool has_required(const std::vector<bool>& required) {
   return std::all_of(required.begin(), required.end(), [](bool flag){ return flag; });
}

void grep(std::vector<std::string> filenames, GrepFactory::lists regexes) {
   std::string buffer;
   const auto is_match = [&buffer](const std::regex& grep){
      return std::regex_search(buffer, grep);
   };
   
   for(const auto& filename : filenames) {
      std::ifstream file(filename);
      std::vector<std::string> line_buffer;
      std::vector<bool> req_matches(regexes.require.size());
      bool rejected = false;
      
      while(std::getline(file, buffer)) {
         if(std::any_of(regexes.reject.begin(), regexes.reject.end(), is_match)) {
            rejected = true;
            break;
         }
         
         for(int i = 0; i < req_matches.size(); ++i) {
            if(req_matches[i]) continue;
            else req_matches[i] = is_match(regexes.require[i]);
         }
         
         if(std::any_of(regexes.match.begin(), regexes.match.end(), is_match)) {
            line_buffer.push_back(buffer);
         }
      }
      
      if(not rejected and has_required(req_matches)) {
         // This is where the different output strategies should go.
         if(filenames.size() > 1) print(line_buffer, filename);
         else print(line_buffer);
      }
   }
}

int main(int argc, const char * argv[]) {
   const auto options = get_options(argc, argv);
   if(argc == 1 or options.count("-h") or options.count("--help")) {
      help();
      exit(EXIT_SUCCESS);
   }
   
   GrepFactory factory;
   factory.set_options(options);
   
   const auto filepaths = glob_files(get_args(argc, argv));
   if(filepaths.empty()) {
      std::cout << "No files to search\n";
      exit(EXIT_FAILURE);
   }
   
   const auto regexes = factory.get_regexes(options);
   if(regexes.match.empty()) {
      std::cout << "No patterns to match (as denoted by the '=')\n";
      exit(EX_USAGE);
   }

   grep(filepaths, regexes);
}
