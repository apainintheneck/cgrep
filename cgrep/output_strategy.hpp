#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <iostream>

class OutputStrategy {
public:
   virtual ~OutputStrategy() = default;
   virtual bool add(const std::string& filename, const std::vector<std::string> lines) const = 0;
};

std::unique_ptr<OutputStrategy> get_output_strategy(const std::map<std::string, std::string>& options, std::ostream& out);
