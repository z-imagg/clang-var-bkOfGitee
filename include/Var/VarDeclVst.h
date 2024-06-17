#ifndef VarDeclVst_H
#define VarDeclVst_H


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
#include "Var/VarTypeDesc.h"

using namespace llvm;
using namespace clang;

/**
 * 插入花括号Visitor
 */
class VarDeclVst
        : public RecursiveASTVisitor<VarDeclVst> {
public:
public:
    //Rewriter:4:  Consumer将Rewriter传递给Visitor
    explicit VarDeclVst(ClGO _clGO,const std::shared_ptr<Rewriter> _mRewriter_ptr)
    //Rewriter:5:  Consumer将Rewriter传递给Visitor, 并由Visitor.mRewriter接收
    :
    clGO(_clGO),
    mRewriter_ptr(_mRewriter_ptr)
    {

    }


//    virtual bool VisitDeclStmt(DeclStmt* decl_k);
    virtual bool TraverseDeclStmt(DeclStmt* decl_k);
    void process_singleDecl(const Decl *singleDecl,VarTypeDesc& varTypeDesc_/*出量*/);
    // , QualType &qualType_, bool& focus_, std::string &typeName_, std::string &varName_
    bool insertAfter_VarDecl( bool useCXX,std::vector<const VarTypeDesc*> &varTypeDescVec,LocId varDeclLocId, SourceLocation varDeclEndLoc );



public:
    const std::shared_ptr<Rewriter> mRewriter_ptr;
    ClGO clGO;

    //变量声明紧后,
    // 一个变量声明紧后插入createVar__语句后,则将该位置塞入此集合,
    //   以保证对该位置只插入一次.
    std::unordered_set<LocId,LocId> VarDeclLocIdSet;

  //函数体左花括号紧后
  // 一个变量声明紧后插入createVar__语句后,则将该变量声明所在函数定义体的左花括号位置塞入此集合,
  //   以标记该函数体内已经被插入createVar__语句, 从而使得 FnVst、RetVst做相应插入.
  std::unordered_set<LocId,LocId> createVar__fnBdLBrcLocIdSet;
};


#endif