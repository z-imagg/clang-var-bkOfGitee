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
#include <clang/Lex/PreprocessingRecord.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/PreprocessorOptions.h>
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
    bool VisitStmt(Stmt *S) {
      return true;
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
  //启用 PreprocessingRecord
  //  DetailedRecord默认为false，若不设置DetailedRecord为true，则 Preprocessor::getPreprocessingRecord 为NULL
  CI.getPreprocessor().getPreprocessorOpts().DetailedRecord = true;
//  CI.getPreprocessor().DisableMacroExpansion=true;
//  CI.getPreprocessor().InMacroArgPreExpansion=true;
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
  CI.getFrontendOpts().ProgramAction=frontend::ActionKind::RewriteMacros;
  CI.getFrontendOpts().ProgramAction=frontend::ActionKind::PrintPreprocessedInput;
  //endregion

  //region 运行自定义Action
  if (!CI.ExecuteAction(*Action)) {
    llvm::errs() << "Clang compilation failed\n";
    return 1;
  }
  //endregion



//  SourceManager &SM = CI.getSourceManager();
//  const LangOptions &LO = CI.getLangOpts();
  Preprocessor & _PP=CI.getPreprocessor();

  PreprocessingRecord *PPRecord = _PP.getPreprocessingRecord();
  std::vector<PreprocessedEntity*> vec(PPRecord->begin(),PPRecord->end());


  const FileID &mainFileID = SM.getMainFileID();

  const SourceLocation &BF = SM.getLocForStartOfFile(mainFileID);
  const SourceLocation &EF = SM.getLocForEndOfFile(mainFileID);

  //位置:  人类不可读的轻量级SourceLocation转为人类可读的PresumedLoc
  const PresumedLoc &BFPr = SM.getPresumedLoc(BF);
  const PresumedLoc &EFPr = SM.getPresumedLoc(EF);

  SourceRange BE(BF,EF);

  auto itPreprocessedEntities = PPRecord->getPreprocessedEntitiesInRange(BE);
  std::vector<PreprocessedEntity*> PreprocessedEntityVec(itPreprocessedEntities.begin(), itPreprocessedEntities.end());

//  for(PreprocessedEntity* entity: PreprocessedEntityVec){//这PreprocessedEntityVec里面没有宏展开后文本
  for(PreprocessedEntity* entity: vec){
    std::cout << entity->getKind() <<"\n";
    if (entity->getKind() == clang::PreprocessedEntity::MacroExpansionKind) {
      clang::MacroExpansion* macroExpansion = static_cast<clang::MacroExpansion*>(entity);
      const std::string &__name = macroExpansion->getName()->getName().str();
      const SourceRange &expansionR = macroExpansion->getSourceRange();

      const PresumedLoc &expansionR_B_pr = SM.getPresumedLoc(expansionR.getBegin());
      const PresumedLoc &expansionR_E_pr = SM.getPresumedLoc(expansionR.getEnd());

      CharSourceRange expansionCR=CharSourceRange::getCharRange(expansionR);

      const PresumedLoc &expansionCR_B_pr = SM.getPresumedLoc(expansionCR.getBegin());
      const PresumedLoc &expansionCR_E_pr = SM.getPresumedLoc(expansionCR.getEnd());


      // Preprocessor::getPreprocessingRecord . PreprocessingRecord::getPreprocessedEntitiesInRange(文件开始, 文件结束) 获取到的文本依然是宏展开前的文本，并不是宏展开后的文本。
      const StringRef &expandedText = clang::Lexer::getSourceText(expansionCR, SM, LO);

      std::cout << "Macro expansion: " << expandedText.str() << std::endl;
    }
  }
  return 0;
}