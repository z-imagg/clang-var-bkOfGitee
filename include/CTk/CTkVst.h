#ifndef CTkVst_H
#define CTkVst_H


#include <clang/Rewrite/Core/Rewriter.h>
#include <set>
#include <clang/Frontend/CompilerInstance.h>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceManager.h"

using namespace llvm;
using namespace clang;

//-----------------------------------------------------------------------------
// RecursiveASTVisitor
//-----------------------------------------------------------------------------
class CTkVst
        : public RecursiveASTVisitor<CTkVst> {
public:
    //Rewriter:4:  Consumer将Rewriter传递给Visitor
    explicit CTkVst(Rewriter &R, ASTContext *Ctx, CompilerInstance &CI, SourceManager& SM)
    //Rewriter:5:  Consumer将Rewriter传递给Visitor, 并由Visitor.mRewriter接收
    : mRewriter(R),
    Ctx(Ctx),
    CI(CI),
    SM(SM)
    {

    }

    static const std::string funcName_TCTk ;//= "X__t_clock_tick";
    static const std::string IncludeStmt_TCTk ; // = "#include \"t_clock_tick.h\"\n";

    /**遍历语句
     *
     * @param stmt
     * @return
     */
//    bool VisitStmt(Stmt *S) { return true; } : grep '(Stmt'  /llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/include/clang/AST/RecursiveASTVisitor.h
    bool processStmt(Stmt *stmt);
//    DEF_TRAVERSE_STMT(CallExpr      : grep '(CallExpr'  /llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/include/clang/AST/RecursiveASTVisitor.h
//    virtual bool VisitCallExpr(CallExpr *callExpr);

    //DEF_TRAVERSE_STMT(CompoundStmt  : grep '(CompoundStmt'  /llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/include/clang/AST/RecursiveASTVisitor.h
    virtual bool TraverseCompoundStmt(CompoundStmt *compoundStmt );
    virtual bool TraverseIfStmt(IfStmt *ifStmt);
    virtual bool TraverseWhileStmt(WhileStmt *whileStmt);
    virtual bool TraverseForStmt(ForStmt *forStmt);
    virtual bool TraverseCXXTryStmt(CXXTryStmt *cxxTryStmt);
    //这里应该有 所有能带块的语句: if块、while块、else块、for块、switch块、try块、catch块...




public:
    //这里是Visitor中的Rewriter，非源头，不要构造Rewriter，只能引用Act中的源头Rewriter.
    // 若这里也构造，将出现两个Rewriter, 则后一个Rewriter写入时会覆盖前一个Rewriter的写入，导致前一个Rewriter的写入丢失。
    //     这里前一个是命名空间中的函数们，后一个是顶层函数们。
    //     即 看起来的现象： 命名空间中的函数们 时钟调用语句 丢失， 而顶层函数们写入的时中调用语句 还在。
    Rewriter & mRewriter;

public:
    ASTContext *Ctx;
    CompilerInstance& CI;
    SourceManager& SM;

};


#endif