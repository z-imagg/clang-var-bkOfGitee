#ifndef VarAstCnsm_H
#define VarAstCnsm_H

#include <clang/Rewrite/Core/Rewriter.h>
#include <iostream>
#include <filesystem>
#include <clang/Frontend/CompilerInstance.h>
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/Comment.h"
#include "clang/AST/CommentVisitor.h"


#include <fmt/core.h>

#include "Var/FnVst.h"
#include "base/Util.h"
#include "base/ClGO.h"
#include "Var/VarDeclVst.h"
#include "Var/RetVst.h"

using namespace llvm;
using namespace clang;

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------


class VarAstCnsm : public ASTConsumer {
public:
    //Rewriter:3:  Action将Rewriter传递给Consumer
    explicit VarAstCnsm(CompilerInstance &_CI, SourceManager &_SM, ASTContext& _astContext,
                        LangOptions &_langOptions,Preprocessor &_PP, const std::shared_ptr<Rewriter> _mRewriter_ptr)
            //Rewriter:4:  Consumer将Rewriter传递给Visitor
            :
            clGO(_CI,_SM, _astContext, _langOptions,_PP),
            varOk(false),
            varDeclVst(clGO,_mRewriter_ptr),
            fnVst(varDeclVst.createVar__fnBdLBrcLocIdSet, clGO,_mRewriter_ptr),
            retVst(varDeclVst.createVar__fnBdLBrcLocIdSet, clGO,_mRewriter_ptr)
            {
      //构造函数
//      _rewriter_ptr->overwriteChangedFiles();//C'正常.
    }

//    virtual bool HandleTopLevelDecl(DeclGroupRef DG) ;

    void HandleTranslationUnit(ASTContext &Ctx) override;


public:
    ClGO clGO;
    //两次HandleTranslationUnit的ASTConsumer只能每次新建，又期望第二次不要发生，只能让标志字段mainFileProcessed写成static
    static bool mainFileProcessed;

    //region 判断是否已经处理过了
    //花括号是否已插入
    bool varOk;
    //endregion

    //region 进行处理：插入花括号
    FnVst fnVst;
    //endregion

    RetVst retVst;

    VarDeclVst varDeclVst;
};


#endif