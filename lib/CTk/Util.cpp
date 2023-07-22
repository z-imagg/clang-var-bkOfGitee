
#include "CTk/Util.h"

#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Frontend/CompilerInstance.h>
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include <clang/AST/ParentMapContext.h>

#include <string>
#include <iostream>


using namespace llvm;
using namespace clang;

void Util::insertIncludeToFileStartByLoc(StringRef includeStmtText,SourceLocation Loc, SourceManager &SM, Rewriter& rewriter){
  FileID fileId = SM.getFileID(Loc);

  insertIncludeToFileStart(includeStmtText,fileId,SM,rewriter);
}

void Util::insertIncludeToFileStart(StringRef includeStmtText,FileID fileId, SourceManager &SM, Rewriter& rewriter)   {
//  SourceManager &SM = Context.getSourceManager();
//  FileID MainFileID = SM.getMainFileID();

//  FileID fileId = SM.getFileID(Loc);
  SourceLocation startLoc = SM.getLocForStartOfFile(fileId);

  const RewriteBuffer *RewriteBuf = rewriter.getRewriteBufferFor(fileId);
  if (!RewriteBuf){
    return;
  }


  rewriter.InsertText(startLoc, includeStmtText, true, true);
}

FunctionDecl* Util::findFuncDecByName(ASTContext *Ctx,std::string functionName){
//    std::string functionName = "calc";

  TranslationUnitDecl* translationUnitDecl=Ctx->getTranslationUnitDecl();
  for(auto decl:translationUnitDecl->decls()){
    if(FunctionDecl* funcDecl = dyn_cast<FunctionDecl>(decl)){
      if(funcDecl->getNameAsString()==functionName){
        return funcDecl;
      }
    }
  }
  return NULL;
}

/**
 * 获取 给定 位置范围 的源码文本
 * @param sourceRange
 * @param sourceManager
 * @param langOptions
 * @return
 */
std::string Util::getSourceTextBySourceRange(SourceRange sourceRange, SourceManager & sourceManager, const LangOptions & langOptions){
  //ref:  https://stackoverflow.com/questions/40596195/pretty-print-statement-to-string-in-clang/40599057#40599057
//  SourceRange sourceRange=S->getSourceRange();
  CharSourceRange charSourceRange=CharSourceRange::getCharRange(sourceRange);
  llvm::StringRef strRefSourceText=Lexer::getSourceText(charSourceRange, sourceManager, langOptions);

  std::string strSourceText=strRefSourceText.str();
  return strSourceText;
}

/**
 * 获取语句所属源文件路径
 */
bool Util::getSourceFilePathOfStmt(const Stmt *S, const SourceManager &SM,StringRef& fn) {
  SourceLocation Loc = S->getBeginLoc();
  Util::getSourceFilePathAtLoc(Loc,SM,fn);
}

/**
 * 获取位置所属源文件路径
 * 获取语句所属源文件路径
 * code by chatgpt on : https://chat.chatgptdemo.net/
 * @param S
 * @param SM
 * @param fn
 * @return
 */
bool Util::getSourceFilePathAtLoc(SourceLocation Loc, const SourceManager &SM,StringRef& fn) {
//  SourceLocation Loc = S->getBeginLoc();
  if (Loc.isValid()) {
    FileID File = SM.getFileID(Loc);
    const FileEntry *FE = SM.getFileEntryForID(File);
    if (FE) {
      fn=FE->getName();
//      llvm::outs() << "Source File Path: " << FE->getName() << "\n";
      return true;
    }
  }
  return false;
}

/**开发用工具 get_FileAndRange_SourceText ： 获得SourceRange的 文件路径、文件中的坐标、源码文本
 *
 * @param sourceRange
 * @param CI
 * @return
 */

std::tuple<std::string,std::string>  Util::get_FileAndRange_SourceText(const SourceRange &sourceRange,CompilerInstance& CI){
  //{开发用
  SourceManager &SM = CI.getSourceManager();
  LangOptions &langOpts = CI.getLangOpts();
//      const SourceRange &sourceRange = Decl->getSourceRange();
  std::string fileAndRange = sourceRange.printToString(SM);
  std::string sourceText = Util::getSourceTextBySourceRange(sourceRange, SM, langOpts);
  return std::tuple<std::string,std::string>(fileAndRange,sourceText);
  //}
}

void Util::printStmt(ASTContext &Ctx, CompilerInstance &CI, std::string tag, std::string title, clang::Stmt *stmt,
                     bool printSourceText) {
  int64_t stmtID = stmt->getID(Ctx);
  SourceManager & SM=CI.getSourceManager();
  const char *stmtClassName = stmt->getStmtClassName();
  Stmt::StmtClass stmtClass = stmt->getStmtClass();
  FileID fileId = SM.getFileID(stmt->getBeginLoc());
  SourceRange sourceRange=stmt->getSourceRange();

  printSourceRange(stmtID,
                   CI,
                   tag, title,
                   fileId, sourceRange,
                   "getStmtClassName", stmtClassName,
                   "getStmtClass", stmtClass,
                   NULL,EMPTY_ENUM_VAL,
                   NULL,EMPTY_ENUM_VAL,
                   printSourceText);

}
void Util::printExpr(ASTContext &Ctx, CompilerInstance &CI, std::string tag, std::string title, clang::Expr *expr,
                     bool printSourceText) {
  int64_t exprID = expr->getID(Ctx);
  SourceManager & SM=CI.getSourceManager();
  const char *stmtClassName = expr->getStmtClassName();
  Stmt::StmtClass stmtClass = expr->getStmtClass();
  ExprValueKind valueKind = expr->getValueKind();
  ExprObjectKind objectKind = expr->getObjectKind();
  FileID fileId = SM.getFileID(expr->getBeginLoc());
  SourceRange sourceRange=expr->getSourceRange();

  printSourceRange(exprID,
                   CI,
                   tag, title,
                   fileId, sourceRange,
                   "getStmtClassName", stmtClassName,
                   "getStmtClass", stmtClass,
                   "getValueKind", valueKind,
                   "getObjectKind", objectKind,
                   printSourceText);

}
void Util::printDecl(ASTContext &Ctx, CompilerInstance &CI, std::string tag, std::string title, clang::Decl *decl,
                     bool printSourceText) {
  int64_t declID = decl->getID();
  unsigned int declGlobalID = decl->getGlobalID();
  SourceManager & SM=CI.getSourceManager();
  const char *kindName = decl->getDeclKindName();
  Decl::Kind kind = decl->getKind();
  FileID fileId = SM.getFileID(decl->getBeginLoc());
  SourceRange sourceRange=decl->getSourceRange();
  printSourceRange(declID,
                   CI,
                   tag,title,
                   fileId,sourceRange,
                   "getDeclKindName",kindName,
                   "getKind",kind,
                   NULL,EMPTY_ENUM_VAL,
                   NULL,EMPTY_ENUM_VAL,
                   printSourceText);

}
void  Util::printSourceRange(int64_t nodeID,
        CompilerInstance& CI,
        std::string tag, std::string title,
        FileID fileId, const SourceRange &sourceRange,
        const char *topCategoryFieldName, const char *topCategoryName,
        const char *topCategoryEnumFieldName, int topCategoryEnum,
        const char *category1FieldName, int category1Enum,
        const char *category2FieldName,int category2Enum,
        bool printSourceText){
  SourceManager & SM=CI.getSourceManager();
  FileID mainFileId = SM.getMainFileID();
//  FileID fileId = SM.getFileID(sourceRange.getBegin());

  const std::tuple<std::string, std::string> & frst = get_FileAndRange_SourceText(sourceRange,CI);
  std::string fileAndRange=std::get<0>(frst);
  std::string sourceText=std::get<1>(frst);


  std::cout
    << tag << ","
    << title
    << ",nodeID:" << nodeID
    << ",文件路径、坐标:" << fileAndRange
    << "," << topCategoryFieldName << ":" << topCategoryName
    << "," << topCategoryEnumFieldName << ":" << topCategoryEnum
    <<  ",mainFileId:" << mainFileId.getHashValue()
    << ",fileId:" << fileId.getHashValue()
    ;
  if(category1Enum!=EMPTY_ENUM_VAL){
  std::cout
    << "," << category1FieldName << ":" << category1Enum
    ;
  }
  if(category2Enum!=EMPTY_ENUM_VAL){
    std::cout
      << "," << category2FieldName << ":" << category2Enum
    ;
  }
  if(printSourceText){
    std::cout <<   ",源码:" << sourceText ;
  }

  std::cout  << std::endl;

}