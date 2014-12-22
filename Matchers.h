#ifndef PEDANT_REGEX_MATCHERS
#define PEDANT_REGEX_MATCHERS
#include <regex>
#include <functional>
#include <llvm/Support/raw_ostream.h>

namespace pedant {
  typedef std::function<bool(std::string)> Matcher;
  Matcher GenerateRegexMatcher(std::string expression) {
    return [expression](std::string data) {
      std::regex regex(expression);
      return std::regex_match(data, regex);
    };
  }

  class MatchHistory {
  public:
    template <typename T>
    using SMap = std::map<std::string, T>;

    MatchHistory(SMap<std::string> required = SMap<std::string>())
        : m_Required(required) {
           init_matchers(m_Matchers);
    }
    void addMatcher(std::string name, Matcher mat) {
      m_Matchers[name] = mat;
    }
    void matchName(std::string type, std::string name) {
      //TODO: Incorporate requirements here, also take a source location arg
      for (auto matcher: m_Matchers) {
        if (matcher.second(name))
          m_Matches[type][matcher.first]++;
      }
    }
    void dump() {
      for (auto& type : m_Matches) {
        llvm::outs() << type.first << "\n";
        for (auto result : type.second) {
          llvm::outs() << '\t'
                       << result.first
                       << '\t'
                       << result.second
                       << '\n';
        }
      }
    }

  private:
    void init_matchers(SMap<Matcher>& map) {
      map["all"] = [](std::string){return true;};
      map["pref_foo"] = GenerateRegexMatcher("foo.*"); //for testing
    }

    SMap<std::string> m_Required;
    SMap<Matcher> m_Matchers;
    typedef SMap<int> matcher_result;
    //^maps matcher name to no. of matches
    SMap<matcher_result> m_Matches;
    //^maps identifier type name to a matcher result
  };


}

#endif
