
#include "Var/UtilBusz.h"
#include "base/UtilMainFile.h"
#include "base/UtilCompoundStmt.h"
#include "base/UtilLineNum.h"
#include "base/UtilTraverseSingleParent.h"
#include "Var/Common.h"
#include "Var/ClConst.h"
#include "base/UtilPrintAstNode.h"
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <iostream>

using namespace clang;
bool UtilBusz::func_of_stmt_isModifiable(const Stmt* stmt, CompoundStmt* & compoundStmt_/*出参*/, SourceLocation &  funcBodyLBraceLoc_/*出参*/, SourceLocation & funcBodyRBraceLoc_/*出参*/, CompilerInstance& CI, SourceManager& SM , ASTContext *Ctx){
  bool isModifiableFuncDecl= false;
  clang::DynTypedNode returnNode=clang::DynTypedNode::create(*stmt);
  clang::DynTypedNode funcNode;
  bool found_funcNode=UtilTraverseSingleParent::do_traverse(returnNode, ClConstAstNodeKind::functionDecl , funcNode/*出量*/, CI, Ctx);
  if(found_funcNode){
    const FunctionDecl* funcDecl=funcNode.get<FunctionDecl>();
    assert(funcDecl!= nullptr);
    isModifiableFuncDecl=UtilBusz::isModifiable_FunctionDecl(funcDecl,compoundStmt_/*出量*/,funcBodyLBraceLoc_/*出量*/, funcBodyRBraceLoc_/*出量*/,  SM);
  }
  return isModifiableFuncDecl;
}
/** 给定函数声明 是否 为 能够被修改的函数
 Modifiable == 能够被修改的函数 == 具有被修改资格的函数
 Modifiable 且 该函数还没被修改 == 应该修改该函数
 * @param cxxMethDecl
 * @param CI
 * @param SM
 * @return
 */
bool UtilBusz::isModifiable_FunctionDecl(const FunctionDecl* cxxMethDecl, CompoundStmt* & compoundStmt_/*出参*/, SourceLocation& funcBodyLBraceLoc_/*出参*/, SourceLocation& funcBodyRBraceLoc_/*出参*/, SourceManager& SM ){

  //跳过非MainFile
  bool _LocFileIDEqMainFileID=UtilMainFile::LocFileIDEqMainFileID(SM,cxxMethDecl->getLocation());
  if(!_LocFileIDEqMainFileID){
    RetFalse_For_NotModifiableFunctionDecl;
  }
  //跳过 default
//  if(Util::funcIsDefault(cxxMethDecl)){
//    RetTrue_to_KeepOuterLoop;
//  }
  //跳过 无函数体
  bool hasBody=cxxMethDecl->hasBody();
  if(!hasBody){
    RetFalse_For_NotModifiableFunctionDecl;
  }
  //跳过 constexpr
  bool _isConstexpr = cxxMethDecl->isConstexpr();
  if(_isConstexpr){
    RetFalse_For_NotModifiableFunctionDecl;
  }

  //获得 函数体、左右花括号
  Stmt* body = cxxMethDecl->getBody();
//  CompoundStmt* compoundStmt;
//  SourceLocation funcBodyLBraceLoc,funcBodyRBraceLoc;

  if(!UtilCompoundStmt::funcBodyAssertIsCompoundThenGetLRBracLoc(body, compoundStmt_/*出量*/, funcBodyLBraceLoc_/*出量*/, funcBodyRBraceLoc_/*出量*/)){
    std::string errMsg=fmt::format("[未意料的可能错误] funcBody is not a CompoundStmt. \n" );
    std::cout<<errMsg;
//    UtilPrintAstNode::printStmt(Ctx, CI, "未意料的可能错误", errMsg, body, true);
    RetFalse_For_OtherErr;//跳过for_each的此次循环体 进入下次循环体
  }


  //跳过 函数体内无语句
  int stmtCntInFuncBody= UtilCompoundStmt::childrenCntOfCompoundStmt(compoundStmt_);
  if(stmtCntInFuncBody<=0){
    RetFalse_For_NotModifiableFunctionDecl;
  }

  //跳过 函数左花括号、右花括号在同一行 且 (todo)函数体内只有一条语句的(难,一个大块复合语句也是一条语句)
  bool funcBodyLRBraceInSameLine=UtilLineNum::isEqSrcLocLineNum(SM, funcBodyLBraceLoc_/*入量*/, funcBodyRBraceLoc_/*入量*/);
  if(funcBodyLRBraceInSameLine){
    RetFalse_For_NotModifiableFunctionDecl;
  }


//Modifiable == 能够被修改的函数 == 具有被修改资格的函数
  RetTrue_For_ModifiableFunctionDecl;
}

/** 获得给定语句所在的函数 (暂时没用到函数get_func_of_stmt)
 */
bool UtilBusz::get_func_of_stmt(const Stmt* stmt, const FunctionDecl* & funcDecl_/*出参*/, CompoundStmt*& compoundStmt_/*出参*/,SourceLocation& funcBodyLBraceLoc_/*出参*/,SourceLocation& funcBodyRBraceLoc_/*出参*/,  CompilerInstance& CI,  ASTContext *Ctx){
  //向上查找函数体定义
  clang::DynTypedNode stmtNode=clang::DynTypedNode::create(*stmt);
  clang::DynTypedNode funcNode;
  bool found_funcNode=UtilTraverseSingleParent::do_traverse(stmtNode, ClConstAstNodeKind::functionDecl , funcNode/*出量*/, CI, Ctx);
  //若找到函数体定义
  if(found_funcNode){
    if(const FunctionDecl* funcDecl=funcNode.get<FunctionDecl>()){
      //若的确是函数体声明

      //回传函数体声明
      funcDecl_=funcDecl;

      //获取函数体左花括号位置
      Stmt* funcBody = funcDecl_->getBody();
      if(!UtilCompoundStmt::funcBodyAssertIsCompoundThenGetLRBracLoc(funcBody, compoundStmt_/*出量*/, funcBodyLBraceLoc_/*出量*/, funcBodyRBraceLoc_/*出量*/)){
        std::string errMsg=fmt::format("[未意料的可能错误__get_func_of_stmt] funcBody is not a CompoundStmt. \n" );
        std::cout<<errMsg;
        //若无函数体 返回false
        RetFalse_For_OtherErr;
      }
      return true;
    }
  }
  //若未找到函数体定义 返回false
  return false;
}