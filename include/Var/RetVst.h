#ifndef RetVst_H
#define RetVst_H


#include <clang/Rewrite/Core/Rewriter.h>
#include <set>
#include <clang/Frontend/CompilerInstance.h>
#include <unordered_set>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"
#include "base/LocId.h"
#include "base/ClGO.h"
#include "Constant.h"

using namespace llvm;
using namespace clang;

/**
 * 先以 clang插件clang-voidFnEndInsertRet 确保  每个void函数末尾都有return语句
 * 再执行本插件
 */
class RetVst
        : public RecursiveASTVisitor<RetVst> {
public:
public:
    //Rewriter:4:  Consumer将Rewriter传递给Visitor
    explicit RetVst(std::unordered_set<LocId,LocId> & createVar__fnBdLBrcLocIdSet, CompilerInstance &CI, const std::shared_ptr<Rewriter> rewriter_ptr, ASTContext *Ctx, SourceManager& SM, LangOptions &_langOptions)
    //Rewriter:5:  Consumer将Rewriter传递给Visitor, 并由Visitor.mRewriter接收
    : createVar__fnBdLBrcLocIdSet(createVar__fnBdLBrcLocIdSet),
      clGO(_clGO)
    {

    }

    bool insert_destroy__Before_fnRet(bool useCXX ,LocId retBgnLocId, SourceLocation retBgnLoc);

    bool TraverseReturnStmt(ReturnStmt *returnStmt);

    void do__modify_me(ReturnStmt *returnStmt,std::string filePath);


public:
    ClGO clGO;

    //return紧前
    std::unordered_set<LocId,LocId> retBgnLocIdSet;

  //已有 createVar__语句 的 函数体
  //    变量声明紧后插入的createVar__语句
  //    引用的VarDeclVst中的同名字段
  std::unordered_set<LocId,LocId>& createVar__fnBdLBrcLocIdSet;
};


#endif