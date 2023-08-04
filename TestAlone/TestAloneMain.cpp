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
#include "clang/Lex/MacroInfo.h"
#include <vector>

#include <iostream>

using namespace clang;



int main() {
  // 创建 Clang 编译实例
  clang::CompilerInstance CI;

  CI.createDiagnostics();
  CI.createFileManager();
  CI.createSourceManager(CI.getFileManager());

  CI.getLangOpts().CPlusPlus = true;

  CI.getTargetOpts().Triple= "x86_64-pc-linux-gnu";

  llvm::Triple triple("x86_64-pc-linux-gnu");
  std::shared_ptr<clang::TargetOptions> targetOpts=std::make_shared<clang::TargetOptions>();
  targetOpts->Triple=triple.str();
  TargetInfo* targetInfo=  TargetInfo::CreateTargetInfo(CI.getDiagnostics(), targetOpts) ;
  CI.setTarget(targetInfo);

  CI.createPreprocessor(clang::TU_Complete);
  CI.getPreprocessor().Initialize(*targetInfo);


  SourceManager& SM=CI.getSourceManager();
  LangOptions &LO = CI.getLangOpts();
  Preprocessor &PP = CI.getPreprocessor();

  //添加诊断
  llvm::raw_ostream &OS = llvm::outs();
  DiagnosticOptions *diagnosticOptions = new clang::DiagnosticOptions();
  clang::TextDiagnosticPrinter *TextDiag = new TextDiagnosticPrinter(OS, diagnosticOptions);
  TextDiag->BeginSourceFile(LO,&PP);
//  TextDiag->EndSourceFile();
  CI.getDiagnostics().setClient(TextDiag);

  // 创建 ASTFrontendAction 实例
  clang::FrontendAction* Action = new clang::SyntaxOnlyAction();

  // 设置输入文件
  CI.getFrontendOpts().Inputs.push_back(clang::FrontendInputFile("/pubx/clang-brc/test_in/test_main.cpp", clang::InputKind(clang::Language::CXX)));

  // 运行 Clang 编译
  if (!CI.ExecuteAction(*Action)) {
    llvm::errs() << "Clang compilation failed\n";
    return 1;
  }

  Preprocessor &_PP = CI.getPreprocessor();
  //注意不能用上面的PP,会报指针问题，重新getPreprocessor得到的_PP可以正常使用

//region 获取宏名、宏展开后文本
//  for (clang::Preprocessor::macro_iterator I = _PP.macro_begin(), E = _PP.macro_end(); I != E; ++I) {
////    const clang::MacroInfo *MI = I->second;
//
//    const clang::IdentifierInfo *II = I->first;
//    const StringRef &IdentifierName = II->getName();
//    const clang::MacroInfo *MI = _PP.getMacroInfo(II);
//    if (MI) {
//    //不显示内置宏
//    if(MI->isBuiltinMacro()){
//      continue;
//    }
//    //只显示本文件中定义的宏，开发用
//    if(!SM.isWrittenInMainFile(MI->getDefinitionLoc())){
//      continue;
//    }
//      // 获取宏展开后的标记序列
//      const clang::Token *tokens = MI->tokens().data();
//      unsigned numTokens = MI->tokens().size();
//
//      // 将标记序列转换为文本
//      std::string macroText;
//      for (unsigned i = 0; i < numTokens; ++i) {
//        const clang::Token &token = tokens[i];
//        StringRef tokenText=clang::Lexer::getSourceText(clang::CharSourceRange::getTokenRange(token.getLocation()), SM, CI.getLangOpts());
//        macroText += tokenText.str();
//      }
//
//      // 处理宏参数
//      if (MI->isFunctionLike()) {
////        const clang::MacroInfo::ArgumentList& args = MI->params();
//        auto args = MI->params();
//        macroText += "(";
//        for (unsigned i = 0; i < args.size(); ++i) {
//          if (i > 0) {
//            macroText += ", ";
//          }
//          macroText += args[i]->getName().str();
//        }
//        macroText += ")";
//      }
//
//      // 打印宏展开后的文本
//      std::cout << "Macro Expansion: " << macroText << std::endl;
//    }
//  }

//endregion

//region 废弃
//  for (clang::Preprocessor::macro_iterator I = _PP.macro_begin(), E = _PP.macro_end(); I != E; ++I) {
//    const clang::IdentifierInfo *II = I->first;
//    const clang::MacroInfo *MI = _PP.getMacroInfo(II);
//    if (MI) {
//      // 获取宏展开后的标记序列
//      const Token * TI = MI->tokens_begin();
//      const Token * TE = MI->tokens_end();
//
//      // 将标记序列转换为文本
//      std::string macroText;
//      for (; TI != TE; ++TI) {
//        const clang::Token &token = *TI;
//        if (token.isAnyIdentifier() && token.getIdentifierInfo() == II) {
//          // 处理宏参数
//          const Token *ParamTI = MI->tokens_begin() + (TI - MI->tokens_begin());
//          auto ParamTE = ParamTI + II->getNumTokens();
//          for (; ParamTI != ParamTE; ++ParamTI) {
//            const clang::Token &paramToken = *ParamTI;
//            macroText += clang::Lexer::getSourceText(clang::CharSourceRange::getTokenRange(paramToken.getLocation()), SM, CI.getLangOpts()).str();
//          }
//          TI = ParamTE - 1; // 跳过参数标记
//        } else {
//          macroText += clang::Lexer::getSourceText(clang::CharSourceRange::getTokenRange(token.getLocation()), SM, CI.getLangOpts()).str();
//        }
//      }
//
//      // 打印宏展开后的文本
//      std::cout << "Macro Expansion: " << macroText << std::endl;
//    }
//  }
//endregion


  //region 废弃
//  for (clang::Preprocessor::macro_iterator I = _PP.macro_begin(), E = _PP.macro_end(); I != E; ++I) {
//    const clang::IdentifierInfo *II = I->first;
//    const clang::MacroInfo *MI = _PP.getMacroInfo(II);
//
//    if (MI) {
//    //不显示内置宏
//    if(MI->isBuiltinMacro()){
//      continue;
//    }
//    //只显示本文件中定义的宏，开发用
//    if(!SM.isWrittenInMainFile(MI->getDefinitionLoc())){
//      continue;
//    }
//
//      // 获取宏展开后的标记序列
//      const Token * TI = MI->tokens_begin();  const Token * TE = MI->tokens_end();
//
//      // 将标记序列转换为文本
//      std::string macroText;
//      for (; TI != TE; ++TI) {
//        const clang::Token &token = *TI;
//        IdentifierInfo * tokenIdentifierInfo=token.getIdentifierInfo();
////        if (token.isAnyIdentifier() /*&& tokenIdentifierInfo == II*/ && tokenIdentifierInfo->getTokenID() ) {
//        if (token.isAnyIdentifier() &&  token.getIdentifierInfo()->isExtensionToken() ) {
////          if (  token.hasIdentifierInfo()  ) {
//          // 处理宏参数
//          const Token * ParamTI = TI + 1;
//          const Token * ParamTE = ParamTI + MI->getNumParams();
//          for (; ParamTI != ParamTE; ++ParamTI) {
//            const clang::Token &paramToken = *ParamTI;
//            auto strxxx=clang::Lexer::getSourceText(clang::CharSourceRange::getTokenRange(paramToken.getLocation()), SM, LO);
//            macroText += strxxx.str();
//          }
//          TI = ParamTE - 1; // 跳过参数标记
//        } else {
//          auto STRXXX=clang::Lexer::getSourceText(clang::CharSourceRange::getTokenRange(token.getLocation()), SM, CI.getLangOpts());
//          macroText += STRXXX.str();
//        }
//      }
//
//      // 打印宏展开后的文本
//      std::cout << "Macro Expansion: " << macroText << std::endl;
//    }
//  }
  //endregion



  // 遍历所有宏
  for (clang::Preprocessor::macro_iterator I = _PP.macro_begin(), E = _PP.macro_end(); I != E; ++I) {
    const clang::IdentifierInfo *II = I->first;
    const clang::MacroInfo *MI = _PP.getMacroInfo(II);
    if (MI) {
        //不显示内置宏
        if(MI->isBuiltinMacro()){
          continue;
        }
        //只显示本文件中定义的宏，开发用
        if(!SM.isWrittenInMainFile(MI->getDefinitionLoc())){
          continue;
        }

      // 获取宏展开后的标记序列
      const Token * TI = MI->tokens_begin();  const Token * TE = MI->tokens_end();

      // 将标记序列转换为文本
      std::string macroText;
      for (; TI != TE; ++TI) {
        const clang::Token &token = *TI;
        macroText += _PP.getSpelling(token);
      }

      // 打印宏展开后的文本
      std::cout << "Macro Expansion: " << macroText << std::endl;
    }
  }
return 0;
}