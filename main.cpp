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
TestOption(
    "test-option",
    cl::desc("This is a test option.\n"),
    cl::cat(PedantCategory));

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...");

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, PedantCategory);
  
  llvm::outs() << TestOption.getValue();

  pedant::MatchHistory mHist({{"class", "CamelCase"}}, true); //fill requirements from options

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
