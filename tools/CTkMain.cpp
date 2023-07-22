#include "CTk/CTkAstCnsm.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"

using namespace llvm;
using namespace clang;

//===----------------------------------------------------------------------===//
// Command line options
//===----------------------------------------------------------------------===//
static llvm::cl::OptionCategory CTkAloneCategory("CTkAlone options");

//===----------------------------------------------------------------------===//
// PluginASTAction
//===----------------------------------------------------------------------===//
class CTkAloneAct : public PluginASTAction {
public:
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    SourceManager& SM=CI.getSourceManager();
    LangOptions &langOpts=CI.getLangOpts();
    ASTContext& astContext=CI.getASTContext();
    //Rewriter:2:  Rewriter构造完，在Action.CreateASTConsumer方法中 调用mRewriter.setSourceMgr后即可正常使用
    mRewriter.setSourceMgr(SM, langOpts);

    //Rewriter:3:  Action将Rewriter传递给Consumer
    return std::make_unique<CTkAstCnsm>(CI, mRewriter,
                                        &astContext, SM, langOpts);
  }


    void EndSourceFileAction() override {
//      mRewriter
//         .getEditBuffer(mRewriter.getSourceMgr().getMainFileID())
//         .write(llvm::outs());

//      mRewriter.overwriteChangedFiles();//修改会影响原始文件
    }
private:
    //Rewriter:0:  Rewriter总是作为Action类中的一个成员字段.
    //Rewriter:1:  Rewriter并不是上层传递下来的，而是自己在这构造的.
    Rewriter mRewriter;//这里是独立运行Act中的Rewriter，是源头，理应构造Rewriter.
};

//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//
int main(int Argc, const char **Argv) {
  Expected<tooling::CommonOptionsParser> eOptParser =
      tooling::CommonOptionsParser::create(Argc, Argv, CTkAloneCategory);
  if (auto E = eOptParser.takeError()) {
    errs() << "Problem constructing CommonOptionsParser "
           << toString(std::move(E)) << '\n';
    return EXIT_FAILURE;
  }
  tooling::ClangTool Tool(eOptParser->getCompilations(),
                                 eOptParser->getSourcePathList());

  return Tool.run(
      tooling::newFrontendActionFactory<CTkAloneAct>().get());
}