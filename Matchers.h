#ifndef PEDANT_REGEX_MATCHERS
#define PEDANT_REGEX_MATCHERS
#include <regex>
#include <functional>

namespace pedant {
  typedef std::function<bool(std::string)> Matcher;
  Matcher GenerateRegexMatcher(std::string expression) {
    return [expression](std::string data) {
      std::regex regex(expression);
      return std::regex_match(data, regex);
    };
  }
  std::map<std::string, Matcher> Matchers = {
    {"all",[](std::string){return true;}},
    {"foo", GenerateRegexMatcher("foo.*")} //just for testing
  };
}

#endif
