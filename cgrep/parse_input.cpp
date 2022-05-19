#include "parse_input.hpp"

std::vector<std::string> get_args(const int argc, const char* argv[]) {
   std::vector<std::string> args;
   for(int i = 1; i < argc; ++i) {
      if(strlen(argv[i]) > 0 and argv[i][0] != '-') {
         args.push_back(argv[i]);
      }
   }
   return args;
}

std::set<std::string> get_options(const int argc, const char* argv[]) {
   std::set<std::string> args;
   for(int i = 1; i < argc; ++i) {
      if(strlen(argv[i]) > 0 and argv[i][0] == '-') {
         args.insert(argv[i]);
      }
   }
   return args;
}
