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

std::pair<std::string, std::string> split_option(const std::string& str) {
   const auto equals_sign = str.find('=');
   
   if(equals_sign == std::string::npos) return {str, ""};
   else return {str.substr(0, equals_sign), str.substr(equals_sign + 1)};
}

std::map<std::string, std::string> get_options(const int argc, const char* argv[]) {
   std::map<std::string, std::string> options;
   for(int i = 1; i < argc; ++i) {
      if(strlen(argv[i]) > 0 and argv[i][0] == '-') {
         const auto [option, arg] = split_option(argv[i]);
         options[option] = arg;
      }
   }
   return options;
}
