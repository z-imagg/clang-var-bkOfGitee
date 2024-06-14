

#ifndef UtilFuncDeclIsModifiable_H
#define UtilFuncDeclIsModifiable_H

#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/Frontend/CompilerInstance.h>
using namespace clang;

class UtilBusz {
public:
/** 给定语句所在函数声明 是否 为 能够被修改的函数
 *
 * @param stmt
 * @param CI
 * @param SM
 * @param Ctx
 * @return
 */
  static bool func_of_stmt_isModifiable(const Stmt* stmt,CompoundStmt* & compoundStmt_/*出参*/, SourceLocation & funcBodyLBraceLoc_/*出参*/, SourceLocation & funcBodyRBraceLoc_/*出参*/, CompilerInstance& CI, SourceManager& SM , ASTContext *Ctx );

  /** 给定函数声明 是否 为 能够被修改的函数
 Modifiable == 能够被修改的函数 == 具有被修改资格的函数
 Modifiable 且 该函数还没被修改 == 应该修改该函数
 * @param cxxMethDecl
 * @param CI
 * @param SM
 * @return
 */
  static bool isModifiable_FunctionDecl(const FunctionDecl* cxxMethDecl, CompoundStmt* & compoundStmt_/*出参*/, SourceLocation & funcBodyLBraceLoc_/*出参*/, SourceLocation & funcBodyRBraceLoc_/*出参*/,   SourceManager& SM );

  /** 获得给定语句所在的函数
   */
  static bool get_func_of_stmt(const Stmt* stmt, const FunctionDecl* & funcDecl_/*出参*/, CompilerInstance& CI,  ASTContext *Ctx);
};


#endif //UtilFuncDeclIsModifiable_H
