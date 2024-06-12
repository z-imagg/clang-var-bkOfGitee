
#include "Var/UtilBusz.h"
#include "base/UtilMainFile.h"
#include "base/UtilCompoundStmt.h"
#include "base/UtilLineNum.h"
#include "Var/Common.h"
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
using namespace clang;
/** 给定函数声明 是否 为 能够被修改的函数
 Modifiable == 能够被修改的函数 == 具有被修改资格的函数
 Modifiable 且 该函数还没被修改 == 应该修改该函数
 * @param cxxMethDecl
 * @param CI
 * @param SM
 * @return
 */
bool UtilBusz::isModifiable_FunctionDecl(CXXMethodDecl* cxxMethDecl, SourceManager& SM ){

  //跳过非MainFile
  bool _LocFileIDEqMainFileID=UtilMainFile::LocFileIDEqMainFileID(SM,cxxMethDecl->getLocation());
  if(!_LocFileIDEqMainFileID){
    RetFalse_NotModifiableFunctionDecl;
  }
  //跳过 default
//  if(Util::funcIsDefault(cxxMethDecl)){
//    RetTrue_to_KeepOuterLoop;
//  }
  //跳过 无函数体
  bool hasBody=cxxMethDecl->hasBody();
  if(!hasBody){
    RetFalse_NotModifiableFunctionDecl;
  }
  //跳过 constexpr
  bool _isConstexpr = cxxMethDecl->isConstexpr();
  if(_isConstexpr){
    RetFalse_NotModifiableFunctionDecl;
  }

  //获得 函数体、左右花括号
  Stmt* body = cxxMethDecl->getBody();
  CompoundStmt* compoundStmt;
  SourceLocation funcBodyLBraceLoc,funcBodyRBraceLoc;
  UtilCompoundStmt::funcBodyIsCompoundThenGetLRBracLoc(body, compoundStmt, funcBodyLBraceLoc,funcBodyRBraceLoc);

  //跳过 函数体内无语句
  int stmtCntInFuncBody= UtilCompoundStmt::childrenCntOfCompoundStmt(compoundStmt);
  if(stmtCntInFuncBody<=0){
    RetFalse_NotModifiableFunctionDecl;
  }

  //跳过 函数左花括号、右花括号在同一行 且 (todo)函数体内只有一条语句的(难,一个大块复合语句也是一条语句)
  bool funcBodyLRBraceInSameLine=UtilLineNum::isEqSrcLocLineNum(SM,funcBodyLBraceLoc,funcBodyRBraceLoc);
  if(funcBodyLRBraceInSameLine){
    RetFalse_NotModifiableFunctionDecl;
  }


//Modifiable == 能够被修改的函数 == 具有被修改资格的函数
  RetTrue_ModifiableFunctionDecl;
}