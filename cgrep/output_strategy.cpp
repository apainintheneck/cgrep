#include "output_strategy.hpp"

class MatchingLinesStrategy : public OutputStrategy {
public:
   MatchingLinesStrategy(std::ostream& out, bool print_line_number) : out_(out), print_line_number_(print_line_number) {};
   bool add(const std::string &filename, const std::vector<line> lines) const override;
private:
   std::ostream& out_;
   bool print_line_number_;
};

bool MatchingLinesStrategy::add(const std::string &filename, const std::vector<line> lines) const {
   if(lines.empty()) return false;
   
   for(const auto& line : lines) {
      out_ << filename << ':';
      if(print_line_number_) out_ << line.number << ':';
      out_ << line.text << '\n';
   }
   return true;
}

class LineCountStrategy : public OutputStrategy {
public:
   LineCountStrategy(std::ostream& out) : out_(out) {};
   bool add(const std::string &filename, const std::vector<line> lines) const override;
private:
   std::ostream& out_;
};

bool LineCountStrategy::add(const std::string &filename, const std::vector<line> lines) const {
   out_ << filename << ':' << lines.size() << '\n';
   return true;
}

class MatchFilesStrategy : public OutputStrategy {
public:
   MatchFilesStrategy(std::ostream& out) : out_(out) {};
   bool add(const std::string &filename, const std::vector<line> lines) const override;
private:
   std::ostream& out_;
};

bool MatchFilesStrategy::add(const std::string &filename, const std::vector<line> lines) const {
   if(lines.empty()) return false;
   
   out_ << filename << '\n';
   return true;
}

class NoMatchFilesStrategy : public OutputStrategy {
public:
   NoMatchFilesStrategy(std::ostream& out) : out_(out) {};
   bool add(const std::string &filename, const std::vector<line> lines) const override;
private:
   std::ostream& out_;
};

bool NoMatchFilesStrategy::add(const std::string &filename, const std::vector<line> lines) const {
   if(not lines.empty()) return false;
   
   out_ << filename << '\n';
   return true;
}

std::unique_ptr<OutputStrategy> OutputStrategy::init(const std::map<std::string, std::string>& options, std::ostream& out) {
   if(options.count("-c") or options.count("--count")) {
      return std::make_unique<LineCountStrategy>(out);
   }
   
   if(options.count("-l") or options.count("--files-with-matches")) {
      return std::make_unique<MatchFilesStrategy>(out);
   }
   
   if(options.count("-L") or options.count("--files-without-matches")) {
      return std::make_unique<NoMatchFilesStrategy>(out);
   }
   
   const bool print_line_number = options.count("-n") or options.count("--line-number");
   
   return std::make_unique<MatchingLinesStrategy>(out, print_line_number);
}
