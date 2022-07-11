#include "util.hpp"

#include <cstdlib>
#include <glob.h>

namespace util {

std::vector<std::string> glob(const std::vector<std::string>& patterns) {
   if(patterns.empty()) return {};
   
   glob_t globbuf;
   globbuf.gl_offs = 0;
   // Set GLOB_MARK to easily be able to differentiate betwen directories and files.
   auto flags = GLOB_DOOFFS | GLOB_TILDE | GLOB_MARK;
   // First time without GLOB_APPEND flag.
   glob(patterns.front().c_str(), flags, NULL, &globbuf);
   // Every following call to glob() with GLOB_APPEND set.
   flags |= GLOB_APPEND;
   for(size_t i = 1; i < patterns.size(); ++i) {
      glob(patterns[i].c_str(), flags, NULL, &globbuf);
   }
   
   std::vector<std::string> matches {globbuf.gl_pathv, globbuf.gl_pathv + globbuf.gl_pathc};
   
   globfree(&globbuf);
   
   return matches;
}

std::vector<std::string> glob_files(const std::vector<std::string>& patterns) {
   auto paths = glob(patterns);
   const auto last = std::remove_if(paths.begin(), paths.end(),
                                    [](const auto& path){ return path.back() == '/'; });
   paths.erase(last, paths.end());
   return paths;
}

std::string expand_path(const std::string& path) {
   if(path.empty()) return "";
   
   if(path.front() == '~') {
      if(path.size() == 1) {
         return std::getenv("HOME");
      } else if (path[1] == '/') {
         return std::string(std::getenv("HOME")) + path.substr(1);
      }
   }
   
   return path;
}

} // namespace util
