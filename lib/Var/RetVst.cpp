#include "Var/RetVst.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "base/Util.h"
#include "base/UtilDebugStmt.h"
#include "base/ASTContextUtil.h"
#include "Var/RangeHasMacroAstVst.h"
#include "Var/CollectIncMacro_PPCb.h"
#include "Var/Common.h"
#include "Var/ClConst.h"
#include "Var/UtilBusz.h"
#include <vector>

#include <fmt/core.h>
#include <iostream>
#include <clang/AST/ParentMapContext.h>

#include "base/MyAssert.h"
#include "Var/Constant.h"
#include "base/UtilParentKind.h"
#include "base/UtilMainFile.h"
#include "base/UtilLocId.h"
#include "base/UtilEnvVar.h"
#include "base/UtilTraverseSingleParent.h"
#include "base/UtilCompoundStmt.h"

using namespace llvm;
using namespace clang;

//在return紧前插入'销毁语句'
bool RetVst::insert_destroy__Before_fnRet(bool useCXX ,LocId retBgnLocId, SourceLocation retBgnLoc ){
    std::string verbose="";
    //环境变量 clangPlgVerbose_Var 控制 是否在注释中输出完整路径_行号_列号
    if(UtilEnvVar::envVarEq("clangPlgVerbose_Var","true")){
        verbose=retBgnLocId.to_string();
    }
    std::string fnName=Constant::fnNameS__destroyVar[useCXX];
//【销毁变量通知】  函数在return紧前 插入 销毁语句'destroyVarLs_inFn__RtCxx(_vdLs);'
    //region 构造插入语句
    std::string cStr_destroy=fmt::format(
            "{}(_vdLs); /* 销毁函数变量列表: {}*/",  // destroyVarLs_inFn__RtCxx 或 destroyVarLs_inFn__RtC00
            fnName, verbose
    );
    llvm::StringRef strRef_destroy(cStr_destroy);
    bool insertResult_destroy=mRewriter_ptr->InsertTextBefore(retBgnLoc , strRef_destroy);
    //endregion

  //记录已插入语句的节点ID们以防重： 即使重复遍历了 但不会重复插入
  retBgnLocIdSet.insert(retBgnLocId);
  return insertResult_destroy;
}

bool RetVst::TraverseReturnStmt(ReturnStmt *returnStmt){
//  UtilDebugStmt::debugStmtByGlobalCounter(returnStmt ,*Ctx,CI);
  //调试说明在该函数内
  
  //跳过非MainFile
  bool _LocFileIDEqMainFileID=UtilMainFile::LocFileIDEqMainFileID(clGO.SM,returnStmt->getBeginLoc());
  if(!_LocFileIDEqMainFileID){
    RetTrue_to_KeepOuterLoop;
  }

  //获取主文件ID,文件路径
  FileID mainFileId;
  std::string filePath;
  UtilMainFile::getMainFileIDMainFilePath(clGO.SM,mainFileId,filePath);

  // 若 该return语句所在函数  为 不应修改的函数 , 则直接退出
  CompoundStmt* compoundStmt;
  SourceLocation funcBodyLBraceLoc,funcBodyRBraceLoc;
  bool isModifiableFuncDecl=UtilBusz::func_of_stmt_isModifiable(returnStmt,compoundStmt/*出量*/,funcBodyLBraceLoc/*出量*/, funcBodyRBraceLoc/*出量*/, clGO.CI,clGO.SM,&clGO.astCtx);
  if(!isModifiableFuncDecl){
    RetTrue_to_KeepOuterLoop;
  }

/////////////////////////对当前节点returnStmt做 自定义处理
  //  Ctx.langOpts.CPlusPlus 估计只能表示当前是用clang++编译的、还是clang编译的, [TODO] 但不能涵盖 'extern c'、'extern c++'造成的语言变化
  do__modify_me(returnStmt, filePath);
  RetTrue_to_KeepOuterLoop;//上层遍历循环保持继续以免跳过后续源码文本
///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点doStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
//粘接直接子节点到递归链条: TODO: 这段不知道怎么写（得获得return xxx; 的xxx中可能的lambda表达式，并遍历该lambda表达式)， 也有可能不用写：
//希望return true能继续遍历子节点吧，因为return中应该可以写lambda，lambada内有更复杂的函数结构
//  Expr *xxx = returnStmt->getRetValue();
}

void RetVst::do__modify_me(ReturnStmt *returnStmt,std::string filePath){

  //获得return语句所在函数体
  const FunctionDecl* funcDecl;
  CompoundStmt* compoundStmt;
  SourceLocation funcBodyLBraceLoc, funcBodyRBraceLoc;
  UtilBusz::get_func_of_stmt(returnStmt, funcDecl/*出量*/,compoundStmt/*出参*/,funcBodyLBraceLoc/*出参*/,funcBodyRBraceLoc/*出参*/,clGO.CI, &clGO.astCtx);
  LocId funcBodyLBraceLocId=LocId::buildFor(filePath,   funcBodyLBraceLoc, clGO.SM);

  bool useCxx=ASTContextUtil::useCxx(& clGO.astCtx);

  //返回语句位置
  const SourceLocation &retBgnLoc = returnStmt->getBeginLoc();
  LocId retBgnLocId=LocId::buildFor(filePath,   retBgnLoc, clGO.SM);

  if(bool parentIsCompound=UtilParentKind::parentIsCompound(&clGO.astCtx,returnStmt)){
    if(UtilLocId::LocIdSetNotContains(retBgnLocIdSet, retBgnLocId)) {
      //防重复, 若 本函数还 没有 插入 destroyVarLs_ 语句，才插入。
      if(UtilLocId::LocIdSetContains(this->createVar__fnBdLBrcLocIdSet, funcBodyLBraceLocId)) {
        //若 本函数 有 关注的变量声明(即已插入createVar__语句),  才插入
        insert_destroy__Before_fnRet(useCxx, retBgnLocId, retBgnLoc);
      }
    }
  }

  return; //Release版 函数末尾始终要有return语句
}


