#include "CTk/CTkVst.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "CTk/Util.h"

using namespace clang;

#include <iostream>
#include <clang/AST/ParentMapContext.h>
#include <fmt/core.h>


using namespace llvm;
using namespace clang;

//-----------------------------------------------------------------------------
// CTkVst implementation
//-----------------------------------------------------------------------------
/*


利用  运行clang++的编译 带上本插件.so  实现 对源文件插入时钟滴答语句:
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++  -Xclang   -load -Xclang /pubx/clang-ctk/cmake-build-debug/lib/libCTk.so  -Xclang   -add-plugin -Xclang  CTk  -c /pubx/clang-ctk/t_clock_tick/test_main.cpp


 */


//std::set<FileID> CTkVst::fileInsertedIncludeStmt;//={};//删除fileInsertedIncludeStmt，不再对间接文件做插入，目前只插入直接文件。
const std::string CTkVst::funcName_TCTk = "X__t_clock_tick";
const std::string CTkVst::IncludeStmt_TCTk = "#include \"t_clock_tick.h\"\n";


static auto _VarDeclAstNodeKind=ASTNodeKind::getFromNodeKind<VarDecl>();

static auto _whileStmtAstNodeKind=ASTNodeKind::getFromNodeKind<WhileStmt>();
static auto _forStmtAstNodeKind=ASTNodeKind::getFromNodeKind<ForStmt>();
static auto _returnStmtAstNodeKind=ASTNodeKind::getFromNodeKind<ReturnStmt>();
static auto _unaryOperatorAstNodeKind=ASTNodeKind::getFromNodeKind<UnaryOperator>();
static auto _implicitCaseExprAstNodeKind=ASTNodeKind::getFromNodeKind<ImplicitCastExpr>();
static auto _CXXStaticCastExprAstNodeKind=ASTNodeKind::getFromNodeKind<CXXStaticCastExpr>();
static auto _ExprAstNodeKind=ASTNodeKind::getFromNodeKind<Expr>();
static auto _CompoundStmtAstNodeKind=ASTNodeKind::getFromNodeKind<CompoundStmt>();











bool CTkVst::insertBefore_X__tick(LifeStep lifeStep, int64_t stmtId, SourceLocation stmtBeginLoc, int stackVarAllocCnt, int stackVarFreeCnt, int heapObjAllocCnt, int heapObjcFreeCnt, const char* whoInserted){
  //region 构造插入语句
  Util::emptyStrIfNullStr(whoInserted);
  std::string cStr_X__tick;
  cStr_X__tick=fmt::format(
      "{}(/*栈生*/{}, /*栈死*/{}, /*堆生*/{}, /*堆死*/{},topFuncSVarCnt);//{}\n",
      CTkVst::funcName_TCTk,
      stackVarAllocCnt,stackVarFreeCnt,heapObjAllocCnt,heapObjcFreeCnt,
      //如果有提供，插入者信息，则放在注释中.
      whoInserted
      );
  //X__t_clock_tick(int stackVarAllocCnt, int stackVarFreeCnt, int heapObjAllocCnt, int heapObjcFreeCnt)
  //"X__t_clock_tick(%d, %d, %d, %d)"
  llvm::StringRef strRef_X__t_clock_tick(cStr_X__tick);
  //endregion

  bool insertResult=mRewriter_ptr->InsertTextBefore(stmtBeginLoc, strRef_X__t_clock_tick);//B.   B处mRewriter和A处mRewriter 地址相同，但A处mRewriter.SourceMgr非空，B处mRewriter为空。

  //记录已插入语句的节点ID们以防重： 即使重复遍历了 但不会重复插入
  if(lifeStep == LifeStep::Alloc){
    allocInsertedNodeIDLs.insert(stmtId);
  }else if(lifeStep == LifeStep::Free){
    freeInsertedNodeIDLs.insert(stmtId);
  }
  return insertResult;
}


bool CTkVst::insertBefore_X__funcReturn( int64_t returnStmtId, SourceLocation stmtBeginLoc , const char* whoInserted){
  return CTkVst::insert_X__funcReturn(true,returnStmtId,stmtBeginLoc,whoInserted);
}
bool CTkVst::insertAfter_X__funcReturn( int64_t funcBodyEndStmtId, SourceLocation funEndStmtEndLoc , const char* whoInserted){
  return CTkVst::insert_X__funcReturn(false,funcBodyEndStmtId,funEndStmtEndLoc,whoInserted);
}
bool CTkVst::insert_X__funcReturn(bool before, int64_t flagStmtId, SourceLocation insertLoc , const char* whoInserted){
  //region 构造插入语句
  Util::emptyStrIfNullStr(whoInserted);
  std::string cStr_inserted=fmt::format(
          "X__funcReturn(topFuncSVarCnt/*函出*/);//{}\n",
          //如果有提供，插入者信息，则放在注释中.
          whoInserted
  );
  llvm::StringRef strRef_inserted(cStr_inserted);
  //endregion

  bool insertResult;
  if(before){
    insertResult=mRewriter_ptr->InsertTextBefore(insertLoc, strRef_inserted);
  }else{
    insertResult=mRewriter_ptr->InsertTextAfter(insertLoc, strRef_inserted);
  }

  //记录已插入语句的节点ID们以防重： 即使重复遍历了 但不会重复插入
  funcReturnInsertedNodeIDLs.insert(flagStmtId);
  return insertResult;
}

bool CTkVst::insertAfter_X__funcEnter(int64_t funcDeclId, SourceLocation funcBodyLBraceLoc , const char* whoInserted){
  Util::emptyStrIfNullStr(whoInserted);
  //region 构造插入语句
  std::string cStr_inserted=fmt::format(
          "int topFuncSVarCnt=0; X__funcEnter(/*函入*/);//{}\n",
          //如果有提供，插入者信息，则放在注释中.
          whoInserted
  );
  llvm::StringRef strRef(cStr_inserted);
  //endregion

  bool insertResult=mRewriter_ptr->InsertTextAfterToken(funcBodyLBraceLoc , strRef);

  //记录已插入语句的节点ID们以防重： 即使重复遍历了 但不会重复插入
  funcEnterInsertedNodeIDLs.insert(funcDeclId);

  return insertResult;
}
//TODO 暂时去掉不必要的打印
//TODO 分配变量个数： 当前语句如果是VarDecl

/**遍历语句
 *
 * @param stmt
 * @return
 */
//TODO 喂给processStmt 就是其前肯定能插入的  ， processStmt 不需要再判断是否能插入的问题了？
//TODO 插入前 需要看该语句ID是否已经被插入（ 还是 看 该位置 是否已经被插入？ ）  这两者没区别。 关键是  理论上 rewrite.overwriteChangedFiles 是在 HandleTranslationUnit 结尾 才发生，    所以 这种判断才没有被破坏  才能用。
//    比如 对if语句前 TraverseCompoundStmt 和 TraverseIfStmt 都会插入 ， 这就重复了
bool CTkVst::processStmt(Stmt *stmt,const char* whoInserted){
  this->mRewriter_ptr->setSourceMgr(this->SM,CI.getLangOpts());

  int64_t stmtId = stmt->getID(*Ctx);

  //region 如果当前语句是return 则  X__funcReturn 会被插入, 不需要插入 滴答语句了
  if(bool stmtIsReturn=Util::isReturnStmtClass(stmt)){
    return false;
  }
  //endregion

  //region 如果 本节点ID 已经被插入语句，则不必插入，直接返回即可。
  //依据已插入语句的节点ID们可防重： 即使此次是重复的遍历， 但不会重复插入
  if(allocInsertedNodeIDLs.count(stmtId) > 0){
    return false;
  }
  //endregion

  const LangOptions & langOpts = CI.getLangOpts();


  SourceLocation beginLoc=stmt->getBeginLoc();

  //region 之前开发，排错用代码
//  int beginLine,beginCol;
//  Util::extractLineAndColumn(SM,beginLoc,beginLine,beginCol);//break CTkVst.cpp:126 if beginLine==891
//  SourceRange sourceRange=stmt->getSourceRange();
//  FileID fileId = SM.getFileID(beginLoc);//C
//  FileID mainFileId = SM.getMainFileID();
//  std::string stmtFileAndRange=sourceRange.printToString(SM);
//获取当前语句S的源码文本
//  std::string stmtSourceText=Util::getSourceTextBySourceRange(stmt->getSourceRange(), SM, langOpts);
//  std::cout << "[#" << stmtSourceText << "#]:{#" << stmtClassName << "#}" ;  //开发用打印

//  const char* stmtClassName = stmt->getStmtClassName();

//  Util::printStmt(*Ctx, CI, "查看_VisitStmt", msg, stmt, false);  //开发用打印

//  std::string fnStr=fn.str();

//  auto parent0 = parentS[0];
  //endregion


  //region 说明: constexpr修饰的函数,无法进入processStmt，即constexpr函数内语句前不会被插入tick语句
///////若某函数 有 constexpr 修饰，则在TraverseCXXMethodDecl|TraverseFunctionDecl中被拒绝 粘接直接子节点到递归链条 ，这样该函数体 无法   经过 TraverseStmt(函数体) ---...--->TraverseCompoundStmt(函数体) 转交， 即   不可能 有  TraverseCompoundStmt(该函数体) ， 即  该该函数体中的每条子语句前都 不会 有机会 被  插入 时钟调用语句.
  //endregion


  //region 跳过非MainFile
  //何为MainFile: 即被clang编译的源文件, 如下:
  // clang -c <MainFile>.cpp
  //何为非MainFile: 即在MainFile中include的文件们
  bool stmtIsInMainFile=Util::LocFileIDEqMainFileID(SM, beginLoc);
  if(!stmtIsInMainFile){
//    Util::printStmt(CI,"查看","暂时不对间接文件插入时钟语句",stmt, true); //开发用打印
    return true;
  }
  //endregion

  Stmt::StmtClass stmtClass = stmt->getStmtClass();



  //region 若当前语句是 没有父亲节点的, 则不处理此语句，直接返回.
  DynTypedNodeList parentS=this->Ctx->getParents(*stmt);
  size_t parentSSize=parentS.size();
  if(parentSSize<=0){
    return true;
  }
  //endregion

  //region 此无业务作用 纯属学习用：若当前语句的父亲节点个数大于1,则打印当前语句源码.
  //clang中: 节点node定义的地方 有个父亲节点， 该节点node 被使用的地方 也叫父亲节点。因此变量经常有多个父亲，但语句应该只有一个父亲节点。
  if(parentSSize>1){
    std::string msg=fmt::format("注意:父节点个数大于1, 为:{}",parentSSize);
    Util::printStmt(*Ctx, CI, "查看", msg, stmt, true);
  }
  //endregion


  //region 若本源文件是系统文件或tick源文件 , 则不处理当前语句，直接返回
  // tick源文件: {t_clock_tick.c,t_clock_tick.cpp,t_clock_tick.h}
  StringRef fn;
  Util::getSourceFilePathOfStmt(stmt, SM, fn);
  //若是系统文件 或 tick文件则跳过
  if(Util::isSysSrcFile(fn)  || Util::isTickSrcFile(fn)){
    return true;
  }
  //endregion

  //region 此时才对当前语句前插入tick语句, 并结束本函数
    int stackVarAllocCnt=0;
    int stackVarFreeCnt=0;
    int heapObjAllocCnt=0;
    int heapObjcFreeCnt=0;
    if(stmtClass==Stmt::StmtClass::DeclStmtClass){//判断269， TODO: 此判断和 判断271 有重复的感觉. 估计可以去掉本判断
      //如果当前语句是声明语句
      if (DeclStmt *declStmt = dyn_cast<DeclStmt>(stmt)) {//判断271
//        DeclStmt *declStmt = static_cast<DeclStmt *>(stmt);
        //取得声明语句declStmt 中声明的变量个数. 比如 声明语句"int x=0,y;"中声明了2个变量
        stackVarAllocCnt=Util::varCntInVarDecl(declStmt);
      }
    }
  bool insertResult=insertBefore_X__tick(
          LifeStep::Alloc,
          stmtId,
          stmt->getBeginLoc(),
          stackVarAllocCnt,
          stackVarFreeCnt,
          heapObjAllocCnt,
          heapObjcFreeCnt,
          whoInserted);


    Util::emptyStrIfNullStr(whoInserted);

    std::string title=fmt::format("{}插入结果:{},RwtPtr:{:x}", whoInserted , insertResult,reinterpret_cast<uintptr_t> (mRewriter_ptr.get() ) );
    //这里打印说明: mRewriter 地址 有两种值。有某个地方再次造了新的Rewriter，导致后一个结果覆盖了前一个结果，前一个结果丢失。应该一直用同一个mRewriter
    Util::printStmt(*Ctx, CI, "插入滴答语句", title, stmt, false);  //开发用打印

  return true;
  //endregion
}


/*bool CTkVst::VisitCallExpr(CallExpr *callExpr){

}*/

bool CTkVst::TraverseCompoundStmt(CompoundStmt *compoundStmt  ){

/////////////////////对当前节点compoundStmt做 自定义处理
  //region 0.准备、开发用语句
  int64_t compoundStmtID = compoundStmt->getID(*Ctx);
  const Stmt::child_range &subStmtLs = compoundStmt->children();

  const std::string &compoundStmtText = Util::getSourceTextBySourceRange(compoundStmt->getSourceRange(), SM, CI.getLangOpts());

  std::vector<clang::Stmt*> subStmtVec(subStmtLs.begin(), subStmtLs.end());
  unsigned long subStmtCnt = subStmtVec.size();
//  const std::vector<std::string> &textVec = Util::stmtLs2TextLs(subStmtVec, SM, CI.getLangOpts());
  //endregion

  //////1. 计算 块尾释放语句 插入位置, 并在该位置 插入 块尾释放语句
  //region 1.1 计算 块内子语句列表 中 变量声明语句个数 ，"1.6" 中用到
  //此组合语句内的变量声明语句个数
  int declStmtCnt=0;

  for(Stmt* subStmt:subStmtLs){
    const char *subStmtClassName = subStmt->getStmtClassName();
    Stmt::StmtClass subStmtClass = subStmt->getStmtClass();
    if(Stmt::DeclStmtClass == subStmtClass){
      DeclStmt* declStmt=static_cast<DeclStmt*> (subStmt);
      //取得声明语句subDeclStmt 中声明的变量个数. 比如 声明语句"int x=0,y;"中声明了2个变量
      declStmtCnt+=Util::varCntInVarDecl(declStmt);
    }
//    Util::printStmt(*Ctx,CI,"查看组合语句内子语句类型","",subStmt,true);
  }
  //endregion

  ///1.2 块尾释放语句默认插入位置是 组合语句 右花括号} 前
  SourceLocation insertLoc=compoundStmt->getRBracLoc();

  //region 1.3 如果块内最后一条语句是FallThrough 块尾释放语句插入位置 改为 块内倒数第二条语句前
  Stmt *endStmt = compoundStmt->body_back();

  Stmt* negativeSecond;
  std::vector<bool> subStmtIsFallThroughVec=Util::subStmtIsFallThroughVec(subStmtLs,negativeSecond,SM,CI.getLangOpts());
  bool  endStmtIsFallThrough=subStmtIsFallThroughVec.empty()? false: subStmtIsFallThroughVec.back();
  if(endStmtIsFallThrough){
    //如果块内最后一条语句是'[[gnu::fallthrough]];',  释放语句 位置 应该在 倒数第二条语句之后， 但是如果 fallthrough 是宏，宏周边位置不准，倒数第二个语句后紧挨着宏左边，不准，这时候为了能进行下去，退一步，插到倒数第二条语句左（如果倒数第二条语句本身是变量定义语句，这时候释放语句位置提前了）
    //  TODO 宏周边位置不准，这个问题需要解决
    insertLoc=negativeSecond->getBeginLoc();
  }
  //endregion

  //region 1.4 计算块尾语句是不是return
  // 如果块内最后一条语句是return 则本块释放被 X__funcReturn 包含了, 不需要插入 块尾释放语句.
  // 如果 块尾部的return前 既有 块尾释放语句 又有 X__funcReturn, 即 多释放了, 多释放的部分 是 本块变量.
  bool compoundEndStmtIsReturn=Util::isReturnStmtClass(endStmt);
  //endregion

  //region 1.4B 计算块语句是不是函数的最后一个块
  bool compoundIsLastBlockOfFunc=Util::isLastCompoundStmt(compoundStmt, *Ctx);
  //endregion

  //region 1.5 本块内有声明变量 且 没有本块没插入过释放语句 且 块尾语句不是return 且 块语句不是函数的最后一个块，才会插入释放语句
  //释放语句 未曾插入过吗？
  bool freeNotInserted=freeInsertedNodeIDLs.count(compoundStmtID) <= 0;
  //若 有 栈变量释放 且 未曾插入过 释放语句，则插入释放语句
  if(declStmtCnt>0 && freeNotInserted && (!compoundEndStmtIsReturn)  && (!compoundIsLastBlockOfFunc) ){
  int stackVarAllocCnt=0;
  int stackVarFreeCnt=declStmtCnt;
  int heapObjAllocCnt=0;
  int heapObjcFreeCnt=0;

//  int insertLine, insertCol;//开发看行号用.
//  Util::extractLineAndColumn(SM,insertLoc,insertLine,insertCol);

  ///1.7  在上面算出的位置处, 插入释放语句
    bool insertResult=insertBefore_X__tick(
            LifeStep::Free,
            compoundStmtID,
            insertLoc,
            stackVarAllocCnt,
            stackVarFreeCnt,
            heapObjAllocCnt,
            heapObjcFreeCnt,
            "TraverseCompoundStmt:块释放");
    std::string title=fmt::format("插入结果:{},RwtPtr:{:x}",    insertResult,reinterpret_cast<uintptr_t> (mRewriter_ptr.get() ) );
    Util::printStmt(*Ctx, CI, "TraverseCompoundStmt插入块释放", title, compoundStmt, false);  //开发用打印

  }
  //endregion

  //region 2. 块内每条语句: FallThrough语句的下一条语句得跳过、根据情况在该语句前是否插入 滴答语句
  //subStmtVec中的stmtJ是否应该跳过
  std::vector<bool> subStmtSkipVec(subStmtVec.size(),false);

  // 使用普通for循环和整数循环下标遍历 child_range
  for (std::size_t j = 0; j < subStmtCnt; ++j) {
    clang::Stmt* stmtJ = subStmtVec[j];
//    Util::printStmt(*Ctx,CI,"查看","组合语句的子语句",stmtJ,true);
    ///2.1 FallThrough语句的下一条语句得跳过
    if(subStmtIsFallThroughVec[j]){
      //如果本行语句是'[[gnu::fallthrough]];'  , 那么下一行前不要插入时钟语句, 否则语法错误.
      int nextStmtIdx=(j+1)%(subStmtCnt+1);
      subStmtSkipVec[nextStmtIdx]=true;
    }
    if(!subStmtSkipVec[j]){
    ///2.2 块内的其余语句 调用 processStmt ： 根据情况在该语句前是否插入 滴答语句
      processStmt(stmtJ, "TraverseCompoundStmt");
    }
  }
  //endregion
///////////////////// 自定义处理 完毕

/////////////////////  将递归链条正确的接好:  对 当前节点compoundStmt 下一层节点stmt们 调用 顶层方法TraverseStmt(stmt)
  //region E. 最后： 粘接直接子节点到递归链条
  for(Stmt* stmt:subStmtLs){
    TraverseStmt  (stmt);
  }

//  Util::printStmt(*Ctx,CI,"查看","组合语句",compoundStmt,false);
  return true;
  //endregion
}


bool CTkVst::TraverseIfStmt(IfStmt *ifStmt){
  /////////////////////////对当前节点compoundStmt做 自定义处理
/*  std::all_of(ifStmt->children().begin(), ifStmt->children().end(),
[this](Stmt* childK){
      Util::printStmt(*this->Ctx,this->CI,"查看VisitIfStmt","的孩子",childK, true);
      return true;
    }
  );*/

  if(ifStmt){
    if(Util::parentIsCompound(Ctx,ifStmt)){
      processStmt(ifStmt,"TraverseIfStmt");
    }
  }

///////////////////// 自定义处理 完毕

////////////////////  将递归链条正确的接好:  对 当前节点ifStmt的下一层节点child:{then,else}  调用顶层方法TraverseStmt(child)
  Stmt *thenStmt = ifStmt->getThen();
  Stmt *elseStmt = ifStmt->getElse();

  if(thenStmt){
    Stmt::StmtClass thenStmtClass = thenStmt->getStmtClass();
//    if(thenStmtClass==Stmt::StmtClass::CompoundStmtClass){
      //这一段可以替代shouldInsert
      /**只有当if的then子语句是 块语句 时, 该 then子语句，才需要 经过 TraverseStmt(thenStmt) ---...--->TraverseCompoundStmt(thenStmt) 转交，在 TraverseCompoundStmt(thenStmt) 中 对 then块中的每条子语句前 插入 时钟调用语句.
       * 形如:
       * if(...)
       * {
       * ...;//这里是 if的then子语句, 是一个块语句，需要 对 then块中的每条子语句前 插入 时钟调用语句.
       * }
       */
      TraverseStmt  (thenStmt);
//    }
    /**否则 if的then子语句 肯定是一个单行语句，无需插入 时钟调用语句.
     * 形如 :
     * if(...)
     *   ...;// 这里是 if的then子语句, 是一个单行语句，无需插入 时钟调用语句.
     */
  }

  if(elseStmt){
    TraverseStmt(elseStmt);
  }

  return true;
}
bool CTkVst::TraverseWhileStmt(WhileStmt *whileStmt){
/////////////////////////对当前节点whileStmt做 自定义处理
  if(Util::parentIsCompound(Ctx,whileStmt)){
    processStmt(whileStmt,"TraverseWhileStmt");
  }
///////////////////// 自定义处理 完毕

////////////////////  将递归链条正确的接好:  对 当前节点whileStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  Stmt *bodyStmt = whileStmt->getBody();
  if(bodyStmt){

    Stmt::StmtClass bodyStmtClass = bodyStmt->getStmtClass();
    if(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass){
      //这一段可以替代shouldInsert
      /**只有当while的循环体是 块语句 时, 该 循环体，才需要 经过 TraverseStmt(循环体) ---...--->TraverseCompoundStmt(循环体) 转交，在 TraverseCompoundStmt(循环体) 中 对 该循环体中的每条子语句前 插入 时钟调用语句.
       * 形如:
       * while(...)
       * {
       * ...;//这里是 while的循环体, 是一个块语句，需要 对 循环体中的每条子语句前 插入 时钟调用语句.
       * }
       */
      TraverseStmt(bodyStmt);
    }
    /**否则 while的循环体 肯定是一个单行语句，无需插入 时钟调用语句.
     * 形如 :
     * while(...)
     *   ...;// 这里是 while的循环体, 是一个单行语句，无需插入 时钟调用语句.
     */
  }
  return true;
}

bool CTkVst::TraverseForStmt(ForStmt *forStmt) {
/////////////////////////对当前节点forStmt做 自定义处理
  if(Util::parentIsCompound(Ctx,forStmt)){
    processStmt(forStmt,"TraverseForStmt");
  }
///////////////////// 自定义处理 完毕

////////////////////  将递归链条正确的接好:  对 当前节点forStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  Stmt *bodyStmt = forStmt->getBody();
  if(bodyStmt){
    Stmt::StmtClass bodyStmtClass = bodyStmt->getStmtClass();
//    if(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass){
      //这一段可以替代shouldInsert
      TraverseStmt(bodyStmt);
//    }
  }
  return true;
}

bool CTkVst::TraverseCXXTryStmt(CXXTryStmt *cxxTryStmt) {

/////////////////////////对当前节点forStmt做 自定义处理
  if(Util::parentIsCompound(Ctx,cxxTryStmt)){
    processStmt(cxxTryStmt,"TraverseCXXTryStmt");
  }
///////////////////// 自定义处理 完毕


////////////////////  将递归链条正确的接好:  对 当前节点cxxTryStmt的下一层节点child:{tryBlock} 调用顶层方法TraverseStmt(child)
  CompoundStmt *tryBlockCompoundStmt = cxxTryStmt->getTryBlock();
  if(tryBlockCompoundStmt){

    Stmt::StmtClass tryBlockCompoundStmtClass = tryBlockCompoundStmt->getStmtClass();
    assert(tryBlockCompoundStmtClass==Stmt::StmtClass::CompoundStmtClass) ;//C++Try的尝试体一定是块语句

    TraverseStmt(tryBlockCompoundStmt);
  }
  return true;
}


bool CTkVst::TraverseCXXCatchStmt(CXXCatchStmt *cxxCatchStmt) {

/////////////////////////对当前节点cxxCatchStmt做 自定义处理
//  processStmt(cxxCatchStmt,"TraverseCXXCatchStmt");//catch整体 前 肯定不能插入
///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点cxxCatchStmt的下一层节点child:{handlerBlock} 调用顶层方法TraverseStmt(child)
  Stmt *handlerBlockStmt = cxxCatchStmt->getHandlerBlock();
  if(handlerBlockStmt){
    Stmt::StmtClass handlerBlockStmtClass = handlerBlockStmt->getStmtClass();
    assert(handlerBlockStmtClass==Stmt::StmtClass::CompoundStmtClass) ;//C++Catch的捕捉体一定是块语句

    TraverseStmt(handlerBlockStmt);
  }
  return true;
}

bool CTkVst::TraverseDoStmt(DoStmt *doStmt) {

/////////////////////////对当前节点doStmt做 自定义处理
  if(Util::parentIsCompound(Ctx,doStmt)){
    processStmt(doStmt,"TraverseDoStmt");
  }
///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点doStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  Stmt *body = doStmt->getBody();
  if(body){
    Stmt::StmtClass bodyStmtClass = body->getStmtClass();
    if(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass){
      //这一段可以替代shouldInsert
      TraverseStmt(body);
    }
  }
  return true;
}

bool CTkVst::TraverseSwitchStmt(SwitchStmt *switchStmt) {
//switchStmt: switch整体:  'switch(v){ case k1:{...}  case k2:{...}  default:{} }'
/////////////////////////对当前节点switchStmt做 自定义处理
  if(Util::parentIsCompound(Ctx,switchStmt)){
    processStmt(switchStmt,"TraverseSwitchStmt");
  }
///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点switchStmt的下一层节点child:{switchBody} 调用顶层方法TraverseStmt(child)
  Stmt *switchBody = switchStmt->getBody();//switchBody：switch体 : 即   '{ case k1:{...}  case k2:{...}  default:{} }'
  if(!switchBody){
    return true;
  }

  Stmt::StmtClass bodyStmtClass = switchBody->getStmtClass();
  assert(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass) ;//switch语句的多情况体 一定是块语句
  //不要遍历switch体 即 不要有 TraverseStmt(switch体), 否则会有 TraverseCompoundStmt(switch体) 即 : 会在case前插入语句，这是错误的。

  for (SwitchCase *switchCaseK = switchStmt->getSwitchCaseList(); switchCaseK != nullptr; switchCaseK = switchCaseK->getNextSwitchCase()) {
    if (CaseStmt *caseKStmt = dyn_cast<CaseStmt>(switchCaseK)) {
      //caseKStmt: case k 整体 : 'case k: {....}'
      Stmt *caseKBody = caseKStmt->getSubStmt();//caseKBody: case k 的 身体 : 即 'case k: {...}'  中的 '{...}'
      if(caseKBody){
        TraverseStmt(caseKBody);
      }
    }
  }

  return true;
}


bool CTkVst::TraverseCaseStmt(CaseStmt *caseStmt) {

/////////////////////////对当前节点caseStmt 不做 自定义处理
///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点caseStmt的下一层节点中的单情况体  调用顶层方法TraverseStmt(单情况体)
                                /////case的其他子节点，比如 'case 值:' 中的 '值' 不做处理。
  Stmt *body = caseStmt->getSubStmt();//不太确定 case.getSubStmt 是 获取case的单情况体
  if(body){
    Stmt::StmtClass bodyStmtClass = body->getStmtClass();
    if(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass){
      //这一段可以替代shouldInsert
      TraverseStmt(body);
    }
  }
  return true;
}


////////////////constexpr

bool CTkVst::TraverseFunctionDecl(FunctionDecl *functionDecl) {
  //TraverseFunctionDecl: 跳过非MainFile
  bool _LocFileIDEqMainFileID=Util::LocFileIDEqMainFileID(SM,functionDecl->getLocation());
  if(!_LocFileIDEqMainFileID){
    return false;
  }

  int64_t funcDeclID = functionDecl->getID();
  const SourceRange &sourceRange = functionDecl->getSourceRange();


  //判断该方法是否有default修饰, 若有, 则不处理.
  //default修饰举例: 'void func( ) = default;' (普通函数的default修饰，貌似没找到例子)
  if(Util::funcIsDefault(functionDecl)){
    return true;
  }
  Stmt* body = functionDecl->getBody();

  bool _isConstexpr = functionDecl->isConstexpr();

  //void函数最后一条语句若不是return，则需在最后一条语句之后插入  函数释放语句

  std::function<FuncDesc( )>   funcDescGetter=[functionDecl](){
      const QualType funcReturnType = functionDecl->getReturnType();
      const bool isaCXXConstructorDecl=isa<CXXConstructorDecl>(*functionDecl);
      Stmt *endStmtOfFuncBody = Util::endStmtOfFunc(functionDecl);
      const SourceLocation funcBodyRBraceLoc = functionDecl->getBodyRBrace();
      FuncDesc funcDesc(funcReturnType,isaCXXConstructorDecl,endStmtOfFuncBody,funcBodyRBraceLoc);

      return funcDesc;
  };


  return this->_Traverse_Func(
          sourceRange,
          funcDescGetter,
          _isConstexpr,
          functionDecl->hasBody(),
          funcDeclID,
          body,
          "TraverseFunctionDecl",
          "TraverseFunctionDecl:void函数尾非return");
}

bool CTkVst::TraverseCXXConstructorDecl(CXXConstructorDecl* cxxConstructorDecl){
  //CXXConstructorDecl: 跳过非MainFile
  bool _LocFileIDEqMainFileID=Util::LocFileIDEqMainFileID(SM,cxxConstructorDecl->getLocation());
  if(!_LocFileIDEqMainFileID){
    return false;
  }

  int64_t funcDeclID = cxxConstructorDecl->getID();
  const SourceRange &sourceRange = cxxConstructorDecl->getSourceRange();

  //判断该方法是否有default修饰, 若有, 则不处理.
  //default修饰举例: 'RuleMatcher( ) = default;'
  if(Util::cxxConstructorIsDefault(cxxConstructorDecl)){
    return true;
  }

  Stmt* body = cxxConstructorDecl->getBody();

  bool _isConstexpr = cxxConstructorDecl->isConstexpr();

  std::function<FuncDesc( )>   fn=[cxxConstructorDecl](){
      const QualType funcReturnType = cxxConstructorDecl->getReturnType();
      const bool isaCXXConstructorDecl=isa<CXXConstructorDecl>(*cxxConstructorDecl);
      Stmt *endStmtOfFuncBody = Util::endStmtOfFunc(cxxConstructorDecl);
      const SourceLocation funcBodyRBraceLoc = cxxConstructorDecl->getBodyRBrace();
      FuncDesc funcDesc(funcReturnType,isaCXXConstructorDecl,endStmtOfFuncBody,funcBodyRBraceLoc);
      return funcDesc;
  };


  return this->_Traverse_Func(
          sourceRange,
          fn,
          _isConstexpr,
          cxxConstructorDecl->hasBody(),
          funcDeclID,
          body,
          "TraverseCXXConstructorDecl",
          "TraverseCXXConstructorDecl:构造函数尾非return");
}

bool CTkVst::TraverseCXXMethodDecl(CXXMethodDecl* cxxMethodDecl){
  //TraverseCXXMethodDecl: 跳过非MainFile
  bool _LocFileIDEqMainFileID=Util::LocFileIDEqMainFileID(SM,cxxMethodDecl->getLocation());
  if(!_LocFileIDEqMainFileID){
    return false;
  }

  int64_t funcDeclID = cxxMethodDecl->getID();
  const SourceRange &sourceRange = cxxMethodDecl->getSourceRange();

  //判断该方法是否有default修饰, 若有, 则不处理.
  //default修饰举例: 'RuleMatcher &operator=(RuleMatcher &&Other) = default;'
  if(Util::funcIsDefault(cxxMethodDecl)){
    return true;
  }
  Stmt* body = cxxMethodDecl->getBody();
  bool _isConstexpr = cxxMethodDecl->isConstexpr();

  std::function<FuncDesc( )>   funcDescGetter=[cxxMethodDecl](){
      const QualType funcReturnType = cxxMethodDecl->getReturnType();
      const bool isaCXXConstructorDecl=isa<CXXConstructorDecl>(*cxxMethodDecl);
      Stmt *endStmtOfFuncBody = Util::endStmtOfFunc(cxxMethodDecl);
      const SourceLocation funcBodyRBraceLoc = cxxMethodDecl->getBodyRBrace();
      FuncDesc funcDesc(funcReturnType,isaCXXConstructorDecl,endStmtOfFuncBody,funcBodyRBraceLoc);
      return funcDesc;
  };

  return this->_Traverse_Func(
          sourceRange,
          funcDescGetter,
          _isConstexpr,
          cxxMethodDecl->hasBody(),
          funcDeclID,
          body,
          "TraverseCXXMethodDecl",
          "TraverseCXXMethodDecl:cpp函数尾非return");
}


bool CTkVst::TraverseLambdaExpr(LambdaExpr *lambdaExpr) {
  //TraverseLambdaExpr: 跳过非MainFile
  bool _LocFileIDEqMainFileID=Util::LocFileIDEqMainFileID(SM,lambdaExpr->getBeginLoc());
  if(!_LocFileIDEqMainFileID){
    return false;
  }
  int64_t funcDeclID = lambdaExpr->getID(*Ctx);
  const SourceRange &sourceRange = lambdaExpr->getSourceRange();

  //lambda不存在default体之说

//  Stmt* body = lambdaExpr->getBody();
  CompoundStmt* body = lambdaExpr->getCompoundStmtBody();

  //lambda不存在Constexpr之说
  bool _isConstexpr = false;





  std::function<FuncDesc( )>   funcDescGetter=[lambdaExpr,body](){

      CXXRecordDecl *cxxRecordDecl = lambdaExpr->getLambdaClass();
      //funcReturnType: 优先lambdaExpr->getLambdaClass()->getLambdaTypeInfo()->getType()，其次lambdaExpr->getCallOperator()->getReturnType()
      QualType funcReturnType;
      TypeSourceInfo *typeSourceInfo=NULL;
      if(cxxRecordDecl && (typeSourceInfo = cxxRecordDecl->getLambdaTypeInfo()) ){
        funcReturnType=typeSourceInfo->getType();
      }else
      if(CXXMethodDecl *cxxMethodDecl=lambdaExpr->getCallOperator()){
        funcReturnType=cxxMethodDecl->getReturnType();
      }


      const bool isaCXXConstructorDecl=false;
      Stmt *endStmtOfFuncBody = Util::endStmtOfCompoundStmt(body);
      const SourceLocation funcBodyRBraceLoc = body->getRBracLoc();
      FuncDesc funcDesc(funcReturnType,isaCXXConstructorDecl,endStmtOfFuncBody,funcBodyRBraceLoc);

      return funcDesc;
  };

  return this->_Traverse_Func(
          sourceRange,
          funcDescGetter,
          _isConstexpr,
          true,
          funcDeclID,
          body,
          "TraverseFunctionDecl",
          "TraverseFunctionDecl:void函数尾非return");
}
bool CTkVst::_Traverse_Func(
  const SourceRange &funcSourceRange,
  std::function<FuncDesc( )> funcDescGetter,
  bool funcIsConstexpr,
  bool hasBody,
  int64_t funcDeclID,
  Stmt *funcBodyStmt,
  const char *whoInsertedFuncEnter,
  const char *whoInsertedFuncReturn)
{

/////////////////////////对当前节点cxxMethodDecl|functionDecl做 自定义处理

  const SourceRange &sourceRange = funcSourceRange;

  //函数体内语句个数.
  int stmtCntInFuncBody=Util::childrenCntOfStmt(funcBodyStmt);

  if(hasBody && funcBodyStmt && (!funcIsConstexpr) &&
  stmtCntInFuncBody > 0 //函数体内至少有一条语句
  ) {

    //region 插入 函数进入语句
    SourceLocation funcBodyLBraceLoc;
    if(Util::funcBodyIsCompoundThenGetLBracLoc(funcBodyStmt, funcBodyLBraceLoc)){
      if(this->funcEnterInsertedNodeIDLs.count(funcDeclID) <= 0){
        //若 本函数还 没有 插入 函数进入语句，才插入。
        insertAfter_X__funcEnter(funcDeclID,funcBodyLBraceLoc,whoInsertedFuncEnter);
      }
    }
    //endregion

    //region void函数、构造函数 结尾语句若不是return，则在函数尾 插入 函数释放语句
    Util::emptyStrIfNullStr(whoInsertedFuncReturn);
    FuncDesc funcDesc=funcDescGetter();
    if(Util::isVoidFuncOrConstructorThenNoEndReturn(funcDesc)){
      int64_t endStmtID = funcDesc.endStmtOfFuncBody->getID(*Ctx);
      insertBefore_X__funcReturn(endStmtID,funcDesc.funcBodyRBraceLoc,whoInsertedFuncReturn);
    }
    //endregion
  }

  bool _isConstexpr = funcIsConstexpr;
///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点cxxMethodDecl|functionDecl的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  Stmt *body = funcBodyStmt;
  if(!_isConstexpr){
    //若此函数 有 constexpr 修饰，则拒绝 粘接直接子节点到递归链条 ，这样该函数体 无法   经过 TraverseStmt(函数体) ---...--->TraverseCompoundStmt(函数体) 转交， 即   不可能 有  TraverseCompoundStmt(该函数体) ， 即  该该函数体中的每条子语句前都 不会 有机会 被  插入 时钟调用语句.
    //    由此 变相实现了  constexpr_func_ls标记, 因此 constexpr_func_ls标记可以删除了.
    if(body){
      TraverseStmt(body);
    }
  }

  return true;

}



bool CTkVst::TraverseReturnStmt(ReturnStmt *returnStmt){
  //TraverseReturnStmt: 跳过非MainFile
  bool _LocFileIDEqMainFileID=Util::LocFileIDEqMainFileID(SM,returnStmt->getBeginLoc());
  if(!_LocFileIDEqMainFileID){
    return false;
  }

/////////////////////////对当前节点returnStmt做 自定义处理

  int64_t returnStmtID = returnStmt->getID(*Ctx);
  const SourceLocation &returnBeginLoc = returnStmt->getBeginLoc();
  if(this->funcReturnInsertedNodeIDLs.count(returnStmtID) > 0){
    //若 已经插入 释放栈当前已分配变量 语句，则不必插入，直接返回即可。
    //依据已插入语句的节点ID们可防重： 即使此次是重复的遍历， 但不会重复插入
    return false;
  }

  //只有return语句直接处于块内时，才处理，否则插入会导致语法错误或语义不同,
  //   比如'if(...) return;' 不应该在return前插入,否则语义不同。
  if(bool parentIsCompound=Util::parentIsCompound(Ctx,returnStmt)){
    insertBefore_X__funcReturn(returnStmtID,returnBeginLoc,"TraverseReturnStmt:函数释放");
  }

///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点doStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
//粘接直接子节点到递归链条: TODO: 这段不知道怎么写（得获得return xxx; 的xxx中可能的lambda表达式，并遍历该lambda表达式)， 也有可能不用写：
//希望return true能继续遍历子节点吧，因为return中应该可以写lambda，lambada内有更复杂的函数结构
  return true;
//  Expr *xxx = returnStmt->getRetValue();
}



