#include <regex>
#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <fstream>
#include <algorithm>

class GrepFactory {
public:
   struct lists {
      std::vector<std::regex> require;
      std::vector<std::regex> reject;
      std::vector<std::regex> match;
   };
   using pattern = std::pair<char, std::string>;
   
   int set_options(int len, const char * args[]);
   lists build_regexes(const std::vector<pattern>& patterns);
private:
   // Helpers
   std::regex_constants::syntax_option_type get_options();
   
   // Options
   bool ignore_case = false;
   bool extended_grep = false;
};

int GrepFactory::set_options(int len, const char * args[]) {
   int i = 0;
   for(;i < len; ++i) {
      if(strcmp(args[i], "-i") or strcmp(args[i], "--ignore-case")) {
         ignore_case = true;
      } else if(strcmp(args[i], "-E") or strcmp(args[i], "--extended-regexp")) {
         extended_grep = true;
      } else {
         break;
      }
   }
   
   return i;
}

GrepFactory::lists GrepFactory::build_regexes(const std::vector<pattern> &patterns) {
   GrepFactory::lists regexes;

   for(const auto& pattern : patterns) {
      switch(pattern.first) {
         case '+':
            regexes.require.push_back(
               std::regex(pattern.second, get_options())
            );
            break;
         case '-':
            regexes.reject.push_back(
               std::regex(pattern.second, get_options())
            );
            break;
         case '=':
            regexes.match.push_back(
               std::regex(pattern.second, get_options())
            );
            break;
      }
   }
   
   return regexes;
}

std::regex_constants::syntax_option_type GrepFactory::get_options() {
   auto options = std::regex::optimize;
   
   if(ignore_case) options |= std::regex::icase;
   
   if(extended_grep) options |= std::regex::egrep;
   else options |= std::regex::grep;
   
   return options;
}

std::vector<std::string> glob(int start, int end, const char * args[]) {
   // Start without globbing just literal strings for now
   if(start >= end) return {};
   else return {args + start, args + end};
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

void grep(std::vector<std::string> filenames, GrepFactory::lists regexes) {
   bool rejected = false;
   std::vector<bool> req_matches(regexes.require.size());
   std::vector<std::string> line_buffer;
   std::string buffer;
   const auto is_match = [&buffer](const std::regex& grep){ return std::regex_search(buffer, grep); };
   
   for(const auto& filename : filenames) {
      std::ifstream file(filename);
      
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
      
      if(rejected or not std::all_of(req_matches.begin(), req_matches.end(), [](bool flag){ return flag; })) {
         rejected = false;
         line_buffer.clear();
         std::fill(req_matches.begin(), req_matches.end(), false);
         continue;
      }
      
      for(const auto& line : line_buffer) {
         if(filenames.size() > 1) {
            std::cout << filename << ": ";
         }
         
         std::cout << line << '\n';
      }
      
      line_buffer.clear();
      req_matches.flip();
   }
}

int main(int argc, const char * argv[]) {
   GrepFactory factory;
   const auto arg_start = factory.set_options(argc, argv);
   
   const auto filenames = glob(arg_start, argc, argv);
   if(filenames.empty()) return 1; // more specific error + msg in the future
   
   const auto patterns = read_patterns();
   if(patterns.empty()) return 1; // more specific error + msg in the future
   
   const auto regexes = factory.build_regexes(patterns);
   
   grep(filenames, regexes);

}
