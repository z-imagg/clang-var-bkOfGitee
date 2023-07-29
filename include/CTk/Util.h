//
// Created by zz on 2023/7/22.
//

#ifndef CLANG_TUTOR_UTIL_H
#define CLANG_TUTOR_UTIL_H


#include <string>

#include <clang/Rewrite/Core/Rewriter.h>
#include <set>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/AST/ParentMapContext.h>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceManager.h"


using namespace llvm;
using namespace clang;

class Util {
public:
    static void copySrcFile(std::string srcFilePath,std::string destRootDir="/tmp/");
    static bool LocFileIDEqMainFileID(SourceManager& SM, SourceLocation Loc);
    static bool isMacroLocation(SourceLocation loc, SourceManager &SM);
    static void saveEditBuffer(const std::shared_ptr<Rewriter> rewriter_ptr, FileID mainFileId, std::string filePath);
    static std::string rewriteBufferToString(const RewriteBuffer &buffer);
    static void
    saveRewriteBuffer(const std::shared_ptr<Rewriter> rewriter_ptr, FileID mainFileId, std::string filePath);
    static bool envVarEq(std::string varName, std::string varValueExpect);

    static void saveRewriteBuffer0(const RewriteBuffer *pRewriteBuffer, std::string filePath, std::string title);
    static bool isLastCompoundStmt(CompoundStmt *stmt, ASTContext &context);
    static FunctionDecl *getContainingFunction(CompoundStmt *stmt, ASTContext &context);
    static  Stmt* endStmtOfFunc(FunctionDecl *funcDecl) ;


    static bool isReturnStmtClass(Stmt *stmt );
    /**计算语句列表中各语句是否为FallThrough，附带返回倒数第二条语句
     *
     * @param subStmtLs
     * @param negativeSecond
     * @return
     */
    static std::vector<bool>  subStmtIsFallThroughVec(const Stmt::child_range &subStmtLs ,Stmt* &negativeSecond ,SourceManager& SM, LangOptions& langOptions);
    /**语句是否有该属性
     *
     * @param stmt
     * @param attrKind
     * @return
     */
    static bool hasAttrKind(Stmt *stmt, attr::Kind attrKind);
    static void extractLineAndColumn(const clang::SourceManager& SM, const clang::SourceLocation& sourceLocation, int& line, int& column);
    static bool parentIsCompound(ASTContext* astContext, const Stmt* currentStmt);
    static bool parentClassEqual(ASTContext* astContext, const Stmt* stmt, Stmt::StmtClass targetClass);
    static bool parentKindIsSame(ASTContext *Ctx, const Stmt* stmt, const ASTNodeKind& kind);
    /**
     * 在声明语句 中 声明的变量个数
     * 比如 :
     * 输入 "int i;",  返回1
     * 输入 "float sum,x=0,y;", 返回3
     * 输入 非变量声明语句,  返回0
     * @param stmt
     * @return
     */
    static int varCntInVarDecl(DeclStmt* stmt);
    static void insertIncludeToFileStart(StringRef includeStmtText,FileID fileId, SourceManager &SM, const std::shared_ptr<Rewriter> rewriter_ptr);
    static void insertIncludeToFileStartByLoc(StringRef includeStmtText,SourceLocation Loc, SourceManager &SM, const std::shared_ptr<Rewriter> rewriter_ptr);
    static bool getSourceFilePathAtLoc(SourceLocation Loc, const SourceManager &SM,StringRef& fn);
    static void getSourceFilePathOfStmt(const Stmt *S, const SourceManager &SM,StringRef& fn);

    static FunctionDecl* findFuncDecByName(ASTContext *Ctx,std::string functionName);
    static std::vector<std::string> stmtLs2TextLs(std::vector<Stmt*> stmtVec, SourceManager & SM, const LangOptions & langOptions);
    static std::string getSourceTextBySourceRange(SourceRange sourceRange, SourceManager & sourceManager, const LangOptions & langOptions);



    static std::tuple<std::string,std::string>  get_FileAndRange_SourceText(const SourceRange &sourceRange,CompilerInstance& CI);
    static void printStmt(ASTContext &Ctx, CompilerInstance &CI, std::string tag, std::string title, clang::Stmt *expr,
                          bool printSourceText);
    static void printExpr(ASTContext &Ctx, CompilerInstance &CI, std::string tag, std::string title, clang::Expr *expr,
                          bool printSourceText = false);
    static void printDecl(ASTContext &Ctx, CompilerInstance &CI, std::string tag, std::string title, clang::Decl *decl,
                          bool printSourceText);
    static void  printSourceRange(int64_t nodeID,
      CompilerInstance& CI,
      std::string tag, std::string title,
      FileID fileId, const SourceRange &sourceRange,
      const char *topCategoryFieldName, const char *topCategoryName,
      const char *topCategoryEnumFieldName, int topCategoryEnum,
      const char *category1FieldName=NULL, int category1Enum=EMPTY_ENUM_VAL,
      const char *category2FieldName=NULL,int category2Enum=EMPTY_ENUM_VAL,
      bool printSourceText=false);
    static const int EMPTY_ENUM_VAL=-1;

};


#endif //CLANG_TUTOR_UTIL_H
