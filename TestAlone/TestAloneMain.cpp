#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Basic/TargetInfo.h"
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <iostream>

#include "base/Util.h"
#include "Brc/RangeHasMacroAstVst.h"
#include "Brc/CollectIncMacro_PPCb.h"


using namespace clang;

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
    CompilerInstance& CI;

    explicit MyASTVisitor(CompilerInstance &_CI, const std::shared_ptr<Rewriter> _rewriter_ptr ) : CI(_CI)  {

    }

};

class MyASTConsumer : public clang::ASTConsumer {
public:
    CompilerInstance &CI;
    MyASTVisitor visitor;
    explicit MyASTConsumer(CompilerInstance &_CI, const std::shared_ptr<Rewriter> _rewriter_ptr)  : CI(_CI) ,visitor(_CI,_rewriter_ptr)  {

    }

    void HandleTranslationUnit(clang::ASTContext &Context) override {
      for (Decl *D : Context.getTranslationUnitDecl()->decls()) {
        visitor.TraverseDecl(D);
      }


//      TranslationUnitDecl *translationUnitDecl = Context.getTranslationUnitDecl();
    }
};

class MyASTFrontendAction : public clang::ASTFrontendAction {
public:
    const std::shared_ptr<Rewriter> mRewriter_ptr=std::make_shared<Rewriter>();//这里是插件Act中的Rewriter，是源头，理应构造Rewriter.


    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile) override {

      SourceManager &SM = CI.getSourceManager();
      LangOptions &langOptions = CI.getLangOpts();
      Preprocessor &PP = CI.getPreprocessor();
      ASTContext &astContext = CI.getASTContext();

      CI.getDiagnostics().setSourceManager(&SM);

      mRewriter_ptr->setSourceMgr(SM, langOptions);

      // Act中 添加 收集#include、#define的 预处理回调
      PP.addPPCallbacks(std::make_unique<CollectIncMacro_PPCb>(CI));

      
      return std::make_unique<MyASTConsumer>(CI,mRewriter_ptr);
    }
};

int main() {
  //region 创建编译器实例
  clang::CompilerInstance CI;
  //endregion

  //region 创建 诊断器、文件管理器、源码管理器
  CI.createDiagnostics();
  CI.createFileManager();
  CI.createSourceManager(CI.getFileManager());
  //endregion

  //region 设置语言为C++
  CI.getLangOpts().CPlusPlus = true;
  //endregion

  //region 设置目标平台
  CI.getTargetOpts().Triple= "x86_64-pc-linux-gnu";

  llvm::Triple triple("x86_64-pc-linux-gnu");
  std::shared_ptr<clang::TargetOptions> targetOpts=std::make_shared<clang::TargetOptions>();
  targetOpts->Triple=triple.str();
  TargetInfo* targetInfo=  TargetInfo::CreateTargetInfo(CI.getDiagnostics(), targetOpts) ;
  CI.setTarget(targetInfo);
  //endregion

  //region 创建预处理器、用目标平台初始化预处理器
  CI.createPreprocessor(clang::TU_Complete);
  CI.getPreprocessor().Initialize(*targetInfo);
  //endregion

  //region 为方便 而有 变量 SM、LO、PP
  SourceManager& SM=CI.getSourceManager();
  LangOptions &LO = CI.getLangOpts();
  Preprocessor &PP = CI.getPreprocessor();
  //endregion


  //region 添加诊断
  llvm::raw_ostream &OS = llvm::outs();
  DiagnosticOptions *diagnosticOptions = new clang::DiagnosticOptions();
  clang::TextDiagnosticPrinter *TextDiag = new TextDiagnosticPrinter(OS, diagnosticOptions);
  TextDiag->BeginSourceFile(LO,&PP);
  CI.getDiagnostics().setClient(TextDiag);
  //endregion

  //region 创建自定义Action 即 自定义clang插件
  clang::FrontendAction* Action = new MyASTFrontendAction();
  //endregion

  //region 设置头文件路径
  HeaderSearchOptions &HSO = CI.getHeaderSearchOpts();
  HSO.AddPath("/usr/include/",clang::frontend::Angled, false, false);
  HSO.AddPath("/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib/clang/15.0.0/include/",clang::frontend::Angled, false, false);
  HSO.AddPath("/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/include/",clang::frontend::Angled, false, false);
  //ResourceDir 应该是有用的，但是不知道如何正确添加
//  HSO.ResourceDir=="/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib/clang/15.0.0";
  //endregion

//  PP.EnterSourceFile()

  //region 添加输入源码文件
  FrontendInputFile srcFile = clang::FrontendInputFile("/pubx/clang-brc/test_in/test_main.cpp", clang::InputKind(clang::Language::CXX));
  CI.getFrontendOpts().Inputs.push_back(srcFile);
  //endregion

  //region 运行自定义Action
  if (!CI.ExecuteAction(*Action)) {
    llvm::errs() << "Clang compilation failed\n";
    return 1;
  }
  //endregion

  PreprocessingRecord *PPRecord = PP.getPreprocessingRecord();
  const FileID &mainFileID = SM.getMainFileID();

  const SourceLocation &BF = SM.getLocForStartOfFile(mainFileID);
  const SourceLocation &EF = SM.getLocForEndOfFile(mainFileID);

  //位置:  人类不可读的轻量级SourceLocation转为人类可读的PresumedLoc
  const PresumedLoc &BFPr = SM.getPresumedLoc(BF);
  const PresumedLoc &EFPr = SM.getPresumedLoc(EF);

  SourceRange BE(BF,EF);

  //一个未知类型的迭代器，获知其中元素类型步骤:
  //  1. 拿到迭代器 类型写成auto
  auto 其中元素类型未知的迭代器 = PPRecord->getPreprocessedEntitiesInRange(BE);
  //  2. 将 *迭代器.begin() 写成一类型为auto的变量,
  //      此时clion会在该变量右侧以灰暗的字提示该变量类型，通常是某种类型的指针
  //      虽然clion提示了类型，但不代表此行代码 中的 *迭代器.begin() 运行不会崩溃，实际上在这里这行代码运行即崩溃
  auto 元素0=*其中元素类型未知的迭代器.begin();
  //      这里元素0的类型是 PreprocessedEntity*
  PreprocessedEntity* ele0=*其中元素类型未知的迭代器.begin();
  //  3. 获得了元素类型后，即可将 该迭代器 转为一向量，
  //      迭代器转为vector，通常不会报错.
  std::vector<PreprocessedEntity*> vec(其中元素类型未知的迭代器.begin(), 其中元素类型未知的迭代器.end());


  return 0;
}