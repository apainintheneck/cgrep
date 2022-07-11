#include <regex>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <sysexits.h>

#include "grep_factory.hpp"
#include "parse_input.hpp"
#include "output_strategy.hpp"
#include "util.hpp"

void help() {
   std::cout <<
R"~(
[cgrep]
-------
Conditionally grep files using multiple
required(+), rejected(-) and matched(=) patterns.
Required and rejected patterns are resolved
before returning matches.
   
[usage]
-------
cgrep [-acEhilLn] [-p=pattern-file] [-o=output-file] [file ...]
   
[options]
---------
-a / --again
   Rerun previous query again.
-c / --count
   Count the number of matched lines in each file.
-E / --extended-regexp
   Sets pattern matching to egrep.
-h / --help
   Shows this page.
-i / --ignore-case
   Sets grep to ignore case when matching.
-l / --files-with-matches
   Lists all files with matched lines.
-L / --files-without-matches
   Lists all files without matched lines in search set.
-n / --line-number
   Each output line is preceded by its relative line number.
-o= / --output-file=
   Writes results to a file.
-p= / --pattern-file=
   Reads grep patterns from a file.
)~";
}

bool has_all(const std::vector<bool>& required) {
   return std::all_of(required.begin(), required.end(), [](bool flag){ return flag; });
}

void grep(const std::vector<std::string>& filenames, const GrepFactory::Patterns& patterns, const OutputStrategy* output) {
   std::string buffer;
   std::vector<line> line_buffer;
   bool found_match = false;
   const auto is_match = [&buffer](const std::regex& grep){
      return std::regex_search(buffer, grep);
   };
   
   for(const auto& filename : filenames) {
      std::ifstream file(filename);
      std::vector<bool> required_matches(patterns.require.size());
      bool rejected = false;
      
      for(size_t line_no = 1; std::getline(file, buffer); ++line_no) {
         // Check rejected patterns
         if(std::any_of(patterns.reject.begin(), patterns.reject.end(), is_match)) {
            rejected = true;
            break;
         }
         
         // Check required patterns
         for(size_t i = 0; i < required_matches.size(); ++i) {
            if(required_matches[i]) continue;
            else required_matches[i] = is_match(patterns.require[i]);
         }
         
         // Check match patterns
         if(std::any_of(patterns.match.begin(), patterns.match.end(), is_match)) {
            line_buffer.push_back({std::move(buffer), line_no});
         }
      }
      
      if(not rejected and has_all(required_matches)) {
         found_match = output->add(filename, line_buffer) or found_match;
      }
      line_buffer.clear();
   }
   
   if(not found_match) exit(EXIT_FAILURE);
}

int main(int argc, const char * argv[]) {
   const auto options = parse_options(argc, argv);
   if(argc == 1 or options.count("-h") or options.count("--help")) {
      help();
      exit(EXIT_SUCCESS);
   }
   
   const auto filepaths = util::glob_files(parse_args(argc, argv));
   if(filepaths.empty()) {
      std::cout << "No files to search\n";
      exit(EXIT_FAILURE);
   }
   
   const auto patterns = GrepFactory(options).get_patterns();
   if(patterns.match.empty()) {
      std::cout << "No patterns to match (as denoted by the '=')\n";
      exit(EX_USAGE);
   }
   
   const auto& out_file_path = parse_out_file_path(options);
   if(out_file_path.empty()) {
      const auto output = OutputStrategy::init(options, std::cout);

      grep(filepaths, patterns, output.get());
   } else {
      std::ofstream out_file(out_file_path);
      if(not out_file.is_open()) {
         std::cout << "Cannot open output file: " << out_file_path << '\n';
         exit(EX_IOERR);
      }
      
      const auto output = OutputStrategy::init(options, out_file);

      grep(filepaths, patterns, output.get());
   }
}
