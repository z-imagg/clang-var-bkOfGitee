#include "Var/FnVst.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "base/Util.h"
#include "base/ASTContextUtil.h"
#include "Var/RangeHasMacroAstVst.h"
#include "Var/CollectIncMacro_PPCb.h"
#include "Var/Common.h"
#include "Var/UtilBusz.h"
#include <vector>

#include <fmt/core.h>
#include <iostream>
#include <clang/AST/ParentMapContext.h>

#include "base/MyAssert.h"
#include "Var/Constant.h"
#include "base/UtilEndStmtOf.h"
#include "base/UtilFuncIsX.h"
#include "base/UtilCompoundStmt.h"
#include "base/UtilLineNum.h"
#include "base/UtilMainFile.h"
#include "base/UtilLocId.h"
#include "base/UtilEnvVar.h"
#include "base/UtilPrintAstNode.h"
#include "base/UtilDebugDecl.h"

using namespace llvm;
using namespace clang;


//在函数体左花括号紧后插入'初始化语句'
bool FnVst::insert_init__After_FnBdLBrc( bool useCXX,LocId fnBdLBrcLocId,std::string funcName, SourceLocation funcBodyLBraceLoc , SourceLocation funcBodyRBraceLoc ){

    std::string fnName=Constant::fnNameS__init_varLs[useCXX];
    std::string verbose="";
    //环境变量 clangPlgVerbose_Var 控制 是否在注释中输出完整路径_行号_列号
    if(UtilEnvVar::envVarEq("clangPlgVerbose_Var","true")){
        verbose=fnBdLBrcLocId.to_string();
    }

//在函数左花括号紧后插入  初始化语句'_VarDeclLs * _vdLs=_init_varLs_inFn__RtCxx("源文件路径","函数名",行号,列号);'

    //region 构造插入语句
    std::string cStr_init=fmt::format(
            "_VarDeclLs * _vdLs={}(\"{}\", \"{}\", {}, {}); /* 初始化函数变量列表, {}*/", // _init_varLs_inFn__RtCxx 或 _init_varLs_inFn__RtC00
            fnName,
            fnBdLBrcLocId.filePath,
            funcName,
            fnBdLBrcLocId.line, fnBdLBrcLocId.column,
            verbose
    );
    llvm::StringRef strRef_init(cStr_init);
    bool insertResult_init=mRewriter_ptr->InsertTextAfterToken(funcBodyLBraceLoc , strRef_init);
    //endregion


    //记录已插入语句的节点ID们以防重： 即使重复遍历了 但不会重复插入
    //用funcEnterLocIdSet的尺寸作为LocationId的计数器
    fnBdLBrcLocIdSet.insert(fnBdLBrcLocId);

    return insertResult_init ;
}

bool FnVst::TraverseFunctionDecl(FunctionDecl *funcDecl) {
    CompoundStmt* compoundStmt;
    SourceLocation funcBodyLBraceLoc,funcBodyRBraceLoc;

    bool isModifiableFuncDecl=UtilBusz::isModifiable_FunctionDecl(funcDecl,compoundStmt/*出量*/,funcBodyLBraceLoc/*出量*/, funcBodyRBraceLoc/*出量*/,  SM);
    // 若 此函数  为 不应修改的函数 , 则直接退出
    if(!isModifiableFuncDecl){
      RetTrue_to_KeepOuterLoop;
    }

    //获取最后一条语句
    Stmt *endStmtOfFuncBody = UtilEndStmtOf::endStmtOfCompoundStmt(compoundStmt);

    //获取主文件ID,文件路径
    FileID mainFileId;
    std::string filePath;
  UtilMainFile::getMainFileIDMainFilePath(clGO.SM,mainFileId,filePath);

    //获取函数名称
    const std::string &funcQualifiedName = funcDecl->getQualifiedNameAsString();
//  funcDecl->getNameAsString();
//  funcDecl->getName();

    //按照左右花括号，构建位置id，防止重复插入
    LocId funcBodyLBraceLocId=LocId::buildFor(filePath, funcBodyLBraceLoc, clGO.SM);
    LocId funcBodyRBraceLocId=LocId::buildFor(filePath, funcBodyRBraceLoc, clGO.SM);

    //获取返回类型
    const QualType funcReturnType = funcDecl->getReturnType();

    bool funcIsStatic = funcDecl->isStatic();
    bool funcIsInline = UtilFuncIsX::funcIsInline(funcDecl);

//    std::string verboseLogMsg=fmt::format("开发查问题日志funcIsStatic_funcIsInline:【{}:{}:{};funcQualifiedName】,funcIsStatic={},funcIsInline={}\n",filePath,funcBodyLBraceLocId.line,funcBodyLBraceLocId.column,funcIsStatic,funcIsInline);
//    std::cout<<verboseLogMsg;

    this->do__modify_me__traverseCompoundStmt(//其中的insertAfter_X__funcEnter内Vst.fnBdLBrcLocIdSet、funcLocId.locationId相互配合使得funcLocId.locationId作为funcLocId.srcFileId局部下的自增数
      funcReturnType,
      false,
      endStmtOfFuncBody,
      funcBodyLBraceLoc,
      funcBodyRBraceLoc,
      funcBodyLBraceLocId,
      funcBodyRBraceLocId,
      compoundStmt,
      funcQualifiedName

    );
    RetTrue_to_KeepOuterLoop;//上层遍历循环保持继续以免跳过后续源码文本

}


bool FnVst::TraverseCXXConstructorDecl(CXXConstructorDecl* cxxCnstrDecl){

    return FnVst::I__TraverseCXXMethodDecl(cxxCnstrDecl, "_CXXConstructorDecl");
}

bool FnVst::TraverseCXXMethodDecl(CXXMethodDecl* cxxMthD){
  //cxxMthD is 'CXXMethodDecl | CXXConstructorDecl | CXXDestructorDecl'

  return FnVst::I__TraverseCXXMethodDecl(cxxMthD, "_CXXMethodDecl");
}

bool FnVst::TraverseCXXConversionDecl(CXXConversionDecl * cxxCnvDecl){
  return FnVst::I__TraverseCXXMethodDecl(cxxCnvDecl,"TraverseCXXConversionDecl");
}

bool FnVst::TraverseCXXDestructorDecl(CXXDestructorDecl * cxxDestructorDecl){
    return FnVst::I__TraverseCXXMethodDecl(cxxDestructorDecl, "_CXXDestructorDecl");
}

bool FnVst::I__TraverseCXXMethodDecl(CXXMethodDecl* cxxMethDecl,const char* who){

  CompoundStmt* compoundStmt;
  SourceLocation funcBodyLBraceLoc,funcBodyRBraceLoc;

  bool isModifiableFuncDecl=UtilBusz::isModifiable_FunctionDecl(cxxMethDecl,compoundStmt/*出量*/,funcBodyLBraceLoc/*出量*/, funcBodyRBraceLoc/*出量*/,  SM);
  // 若 此函数  为 不应修改的函数 , 则直接退出
  if(!isModifiableFuncDecl){
    RetTrue_to_KeepOuterLoop;
  }

  //获取最后一条语句
  Stmt *endStmtOfFuncBody = UtilEndStmtOf::endStmtOfCompoundStmt(compoundStmt);

  //获取主文件ID,文件路径
  FileID mainFileId;
  std::string filePath;
  UtilMainFile::getMainFileIDMainFilePath(clGO.SM,mainFileId,filePath);

  //获取函数名称
  const std::string &funcQualifiedName = cxxMethDecl->getQualifiedNameAsString();

  //按照左右花括号，构建位置id，防止重复插入
  LocId funcBodyLBraceLocId=LocId::buildFor(filePath,funcBodyLBraceLoc, clGO.SM);
  LocId funcBodyRBraceLocId=LocId::buildFor(filePath,funcBodyRBraceLoc, clGO.SM);

  //获取返回类型
  const QualType funcReturnType = cxxMethDecl->getReturnType();

  std::string whoReturn=fmt::format("{}:cpp函数尾非return", who);
  this->do__modify_me__traverseCompoundStmt(
    funcReturnType,
    false,
    endStmtOfFuncBody,
    funcBodyLBraceLoc,
    funcBodyRBraceLoc,
    funcBodyLBraceLocId, funcBodyRBraceLocId,
    compoundStmt,
    funcQualifiedName
  );
  RetTrue_to_KeepOuterLoop;//上层遍历循环保持继续以免跳过后续源码文本
}
bool FnVst::TraverseLambdaExpr(LambdaExpr *lambdaExpr) {

  if(sizeof(lambdaExpr)<0){//开发用, 这行的小于号改为大于号 可以暂时跳过lambda
    RetTrue_to_KeepOuterLoop;
  }

  //跳过非MainFile
  bool _LocFileIDEqMainFileID=UtilMainFile::LocFileIDEqMainFileID(clGO.SM,lambdaExpr->getBeginLoc());
  if(!_LocFileIDEqMainFileID){
    RetTrue_to_KeepOuterLoop;
  }
  //跳过 default
  //跳过 无函数体
  //跳过 constexpr

  //遍历lambda函数的capture例子, 但 不太关注capture
  for (const auto& capture : lambdaExpr->captures()) {
    if (
//        capture.getCaptureKind() != clang::LCK_This ||
        capture.getCaptureKind() == clang::LCK_ByCopy ||
        capture.getCaptureKind() == clang::LCK_ByRef
    ) {
      //不太关注capture
    }
  }

  //获得 函数体、左右花括号
//  Stmt* body  = funcDecl->getBody();
  CompoundStmt* compoundStmt = lambdaExpr->getCompoundStmtBody();
//  CompoundStmt* compoundStmt;
  SourceLocation funcBodyLBraceLoc,funcBodyRBraceLoc;
  UtilCompoundStmt::GetCompoundLRBracLoc( compoundStmt, funcBodyLBraceLoc,funcBodyRBraceLoc);

  //跳过 函数体内无语句
  int stmtCntInFuncBody= UtilCompoundStmt::childrenCntOfCompoundStmt(compoundStmt);
  if(stmtCntInFuncBody<=0){
    RetTrue_to_KeepOuterLoop;
  }

  //跳过 函数左花括号、右花括号在同一行 且 (todo)函数体内只有一条语句的(难,一个大块复合语句也是一条语句)
  //  lambda表达式写在一行是很常见的, 因此要求该lambda表达式体内无语句时才跳过
  bool funcBodyLRBraceInSameLine=UtilLineNum::isEqSrcLocLineNum(clGO.SM,funcBodyLBraceLoc,funcBodyRBraceLoc);
  if(funcBodyLRBraceInSameLine && stmtCntInFuncBody == 0){
    RetTrue_to_KeepOuterLoop;
  }

  //获取最后一条语句
  Stmt *endStmtOfFuncBody = UtilEndStmtOf::endStmtOfCompoundStmt(compoundStmt);

  //获取主文件ID,文件路径
  FileID mainFileId;
  std::string filePath;
  UtilMainFile::getMainFileIDMainFilePath(clGO.SM,mainFileId,filePath);

  //lambda无函数名称
  const char * funName="";

  //按照左右花括号，构建位置id，防止重复插入
  LocId funcBodyLBraceLocId=LocId::buildFor(filePath, funcBodyLBraceLoc, clGO.SM);
  LocId funcBodyRBraceLocId=LocId::buildFor(filePath, funcBodyRBraceLoc, clGO.SM);

  //获取返回类型
  CXXRecordDecl *cxxRecordDecl = lambdaExpr->getLambdaClass();
  // funcReturnType:
  //        优先lambdaExpr->getLambdaClass()->getLambdaTypeInfo()->getType()，
  //        其次lambdaExpr->getCallOperator()->getReturnType()
  QualType funcReturnType;
  TypeSourceInfo *typeSourceInfo=NULL;
  if(cxxRecordDecl && (typeSourceInfo = cxxRecordDecl->getLambdaTypeInfo()) ){
    funcReturnType=typeSourceInfo->getType();
  }else
  if(CXXMethodDecl *cxxMethodDecl=lambdaExpr->getCallOperator()){
    funcReturnType=cxxMethodDecl->getReturnType();
  }

  //lambda一定有body

  this->do__modify_me__traverseCompoundStmt(
    funcReturnType,
    false,
    endStmtOfFuncBody,
    funcBodyLBraceLoc,
    funcBodyRBraceLoc,
    funcBodyLBraceLocId, funcBodyRBraceLocId,
    compoundStmt,
    funName
  );

  RetTrue_to_KeepOuterLoop;//上层遍历循环保持继续以免跳过后续源码文本

}
void FnVst::do__modify_me__traverseCompoundStmt(
//        bool funcIsStatic,
//        bool funcIsInline,
        QualType funcReturnType,
        bool isaCXXConstructorDecl,
        Stmt *endStmtOfFuncBody,
        SourceLocation funcBodyLBraceLoc,
        SourceLocation funcBodyRBraceLoc,
        LocId funcBodyLBraceLocId,
        LocId funcBodyRBraceLocId,
        CompoundStmt* compoundStmt,
        std::string funcName
//  bool funcIsConstexpr,
//  bool hasBody,
//  int64_t funcDeclID,
//  Stmt *funcBodyStmt,
)
{
//    Util::printStmt(*Ctx,CI,"_Traverse_Func","查看语句compoundStmt源码",compoundStmt,true);

/////////////////////////对当前节点cxxMethodDecl|functionDecl做 自定义处理
  bool useCxx = ASTContextUtil::useCxx(& clGO.astCtx);

    //region 插入 函数进入语句
    if(UtilLocId::LocIdSetNotContains(fnBdLBrcLocIdSet, funcBodyLBraceLocId)){
      //防重复, 若 本函数还 没有 插入 _init_varLs_ 语句，才插入。
      //        Util::printStmt(*Ctx, CI, fmt::format("排查问题:{:x},",reinterpret_cast<uintptr_t> (&fnBdLBrcLocIdSet)), funcBodyLBraceLocId.to_csv_line(), compoundStmt, true);
      if(UtilLocId::LocIdSetContains(this->createVar__fnBdLBrcLocIdSet, funcBodyLBraceLocId)) {
      //若 本函数 有 关注的变量声明(即已插入createVar__语句),  才插入
        insert_init__After_FnBdLBrc(useCxx,funcBodyLBraceLocId,funcName, funcBodyLBraceLoc, funcBodyRBraceLoc);
      }
    }
//    }
    //endregion


///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点cxxMethodDecl|functionDecl的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  // 粘接直接子节点到递归链条
  if(compoundStmt){
    TraverseStmt(compoundStmt);
  }

    return; //Release版 函数末尾始终要有return语句
}


