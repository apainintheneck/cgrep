#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <iostream>

struct line {
   std::string text;
   size_t number;
};

class OutputStrategy {
public:
   static std::unique_ptr<OutputStrategy> init(const std::map<std::string, std::string>& options, std::ostream& out);
   virtual ~OutputStrategy() = default;
   virtual bool add(const std::string& filename, const std::vector<line> lines) const = 0;
};
