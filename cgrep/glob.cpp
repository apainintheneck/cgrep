#include "glob.hpp"

#include <glob.h>

std::vector<std::string> glob(const std::vector<std::string>& patterns) {
   if(patterns.empty()) return {};
   
   glob_t globbuf;
   auto flags = GLOB_DOOFFS | GLOB_TILDE | GLOB_BRACE;
   // First time without GLOB_APPEND flag.
   glob(patterns.front().c_str(), flags, NULL, &globbuf);
   // Every following call to glob() with GLOB_APPEND set.
   flags |= GLOB_APPEND;
   for(int i = 1; i < patterns.size(); ++i) {
      glob(patterns[i].c_str(), flags, NULL, &globbuf);
   }
   
   std::vector<std::string> matches{globbuf.gl_pathv, globbuf.gl_pathv + globbuf.gl_pathc};
   
   globfree(&globbuf);
   
   return matches;
}
