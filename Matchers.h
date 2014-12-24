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
  Matcher GeneratePrefixMatcher(std::string prefix) {
    return [prefix](std::string data) {
      return llvm::StringRef(data).startswith(prefix);
    };
  }

  std::map<std::string, Matcher> getStandardMatchers() {
    std::map<std::string, Matcher> map;
      map["all"] = [](std::string){return true;};
      map["none"] = [](std::string){return false;};
      map["fooprefixed"] = GeneratePrefixMatcher("foo"); //for testing
      map["CamelCase"] = GenerateRegexMatcher("([A-Z].*)+");
      map["camelCaseLite"] = GenerateRegexMatcher("[^A-Z]+([A-Z].*)+");
      map["usep_lcase"] = GenerateRegexMatcher("([^A-Z]+)(_[^A-Z]+)*");
    return map;
  }

  extern std::map<std::string, Matcher> StandardMatchers; //TODO: use these later

  class MatchHistory {
  public:
    template <typename T>
    using SMap = std::map<std::string, T>;

    MatchHistory(bool onlyReq = false)
        :m_OnlyReq(onlyReq) {
      if (!onlyReq)
        m_Matchers = getStandardMatchers();
    }
    void addMatcher(std::string name, Matcher mat) {
      m_Matchers[name] = mat;
    }
    void addRequirement(std::string type, std::string matcherName) {
      //TODO: check if matcher exists
      m_Required[type] = matcherName;
    }

    void matchName(std::string type, std::string name) {
      if (m_OnlyReq) {
        auto it = m_Required.find(type);
        std::string matcher = it->second;
        if (it != m_Required.end())
          if(!m_Matchers[matcher](name))
            llvm::outs() << "FAIL: " << type << "\t" << matcher << "\t" << name << "\n";
      }
      else for (auto matcher: m_Matchers) {
        if (matcher.second(name)) {
//          llvm::outs() << matcher.first <<' '<<name<<"\n";
          m_Matches[type][matcher.first]++;
        }
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

    SMap<std::string> m_Required;
    SMap<Matcher> m_Matchers;
    typedef SMap<int> matcher_result;
    //^maps matcher name to no. of matches
    SMap<matcher_result> m_Matches;
    //^maps identifier type name to a matcher result
    bool m_OnlyReq;
    //^Only match requirements
  };


}

#endif
