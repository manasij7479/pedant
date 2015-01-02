// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"

#include "PedantAction.h"
#include "Matchers.h"
using namespace clang::tooling;
using namespace llvm;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory PedantCategory("pedant options");

static cl::opt<std::string>
ClassNames(
    "class",
    cl::desc("Matcher for class names"),
    cl::cat(PedantCategory));

static cl::opt<std::string>
FunctionNames(
    "function",
    cl::desc("Matcher for function names"),
    cl::cat(PedantCategory));

static cl::opt<std::string>
MethodNames(
    "method",
    cl::desc("Matcher for member function/method names"),
    cl::cat(PedantCategory));

static cl::opt<std::string>
MemberNames(
    "member",
    cl::desc("Matcher for member variable/object names"),
    cl::cat(PedantCategory));


// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...");

void parseReqs(pedant::MatchHistory& mHist) {
  //TODO: sanitize user inputs
  std::map<std::string,std::string> reqMap;
  if (ClassNames.getNumOccurrences() != 0 ){
    std::string matcher = ClassNames.getValue();
    reqMap["class"] = matcher;
  }
  if (FunctionNames.getNumOccurrences() != 0 ){
    std::string matcher = FunctionNames.getValue();
    reqMap["function"] = matcher;
  }
  if (MethodNames.getNumOccurrences() != 0 ){
    std::string matcher = MethodNames.getValue();
    reqMap["method"] = matcher;
  }
  if (MemberNames.getNumOccurrences() != 0 ){
    std::string matcher = MemberNames.getValue();
    reqMap["member"] = matcher;
  }
  auto&& stdMatchers = pedant::getStandardMatchers();
  for(auto&& req: reqMap) {
    auto it = stdMatchers.find(req.second);
    if (it != stdMatchers.end()) {
      mHist.addMatcher(req.second, it->second);
      mHist.addRequirement(req.first, req.second);
    }
    else {
      llvm::StringRef str = req.second;
      const std::string prefix_op = "prefix-";
      if (str.startswith(prefix_op)) {
        std::string prefix = str.substr(prefix_op.length());
        mHist.addMatcher(req.second, pedant::GeneratePrefixMatcher(prefix));
        mHist.addRequirement(req.first, req.second);
      }
      const std::string regex_op = "regex-";
      if (str.startswith(regex_op)) {
        std::string regex = str.substr(regex_op.length());
        llvm::outs() << "INPUT:" <<regex<<"\n";
        mHist.addMatcher(req.second, pedant::GenerateRegexMatcher(regex));
        mHist.addRequirement(req.first, req.second);
      }
      //TODO: ERROR
    }
  }
}

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, PedantCategory);


  pedant::MatchHistory mHist(true);
  parseReqs(mHist);//fill requirements from options

  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  class PedantFrontendActionFactory : public FrontendActionFactory {
  public:
    PedantFrontendActionFactory(pedant::MatchHistory& mh):m_Hist(mh){}
    clang::FrontendAction *create() override { return new PedantAction(m_Hist); }
  private:
    pedant::MatchHistory& m_Hist;
  };

  auto result = Tool.run(new PedantFrontendActionFactory(mHist));
  mHist.dump();
  return result;
}
