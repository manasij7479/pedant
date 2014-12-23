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

std::map<std::string,std::string> parseReqs() {
  //TODO: sanitize user inputs
  std::map<std::string,std::string> result;
  if (ClassNames.getNumOccurrences() != 0 ){
    std::string matcher = ClassNames.getValue();
    result["class"] = matcher;
  }
  if (FunctionNames.getNumOccurrences() != 0 ){
    std::string matcher = FunctionNames.getValue();
    result["function"] = matcher;
  }
  if (MethodNames.getNumOccurrences() != 0 ){
    std::string matcher = MethodNames.getValue();
    result["method"] = matcher;
  }
  if (MemberNames.getNumOccurrences() != 0 ){
    std::string matcher = MemberNames.getValue();
    result["member"] = matcher;
  }
  return result;
}

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, PedantCategory);

  pedant::MatchHistory mHist(parseReqs(), true); //fill requirements from options

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
