#include "CodeStyleChecker/CodeStyleCheckerVisitor.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

#include <iostream>
#include <clang/AST/ParentMapContext.h>

//-----------------------------------------------------------------------------
// CodeStyleCheckerVisitor implementation
//-----------------------------------------------------------------------------
/*
运行clang++带上本插件.so：但这只是cc1  如何能把整个编译过程串起来？
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++ -cc1  -load /pubx/clang-tutor/cmake-build-debug/lib/libCodeStyleChecker.so   -plugin CSC   test_main.cpp


只运行了本插件CSC，没有运行编译过程:
#参考: https://releases.llvm.org/8.0.0/tools/clang/docs/ClangPlugins.html
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++  -Xclang   -load -Xclang /pubx/clang-tutor/cmake-build-debug/lib/libCodeStyleChecker.so  -Xclang   -plugin -Xclang  CSC  -c  /pubx/clang-tutor/test/test_main.cpp

"-plugin" 改为  "-add-plugin", 运行了编译过程:  并输出了 test_main.o
#参考: https://www.ibm.com/docs/en/xl-c-and-cpp-linux/16.1.0?topic=cla-running-user-defined-actions-by-using-clang-plug-ins
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++  -Xclang   -load -Xclang /pubx/clang-tutor/cmake-build-debug/lib/libCodeStyleChecker.so  -Xclang   -add-plugin -Xclang  CSC  -c  /pubx/clang-tutor/test/test_main.cpp


运行clang++带上本插件.so 且 运行编译、链接 全过程:
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++  -Xclang   -load -Xclang /pubx/clang-tutor/cmake-build-debug/lib/libCodeStyleChecker.so  -Xclang   -add-plugin -Xclang  CSC   /pubx/clang-tutor/test/test_main.cpp  -o test_main
但运行应用，应用结束时 t没变依然是0，说明本插件对源码的修改没生效.



 */


std::set<clang::FileID> CodeStyleCheckerVisitor::fileInsertedIncludeStmt;//={};
const std::string CodeStyleCheckerVisitor::funcName_TCTick = "X__t_clock_tick";
const std::string CodeStyleCheckerVisitor::IncludeStmt_TCTick = "#include \"t_clock_tick.h\"\n";

bool CodeStyleCheckerVisitor::VisitCallExpr(clang::CallExpr *callExpr){

}

void CodeStyleCheckerVisitor::insertIncludeToFileStartByLoc(clang::SourceLocation Loc, clang::SourceManager &SM, clang::Rewriter& rewriter){
  FileID fileId = SM.getFileID(Loc);

  insertIncludeToFileStart(fileId,SM,rewriter);
}
void CodeStyleCheckerVisitor::insertIncludeToFileStart(FileID fileId, clang::SourceManager &SM, clang::Rewriter& rewriter)   {
//  clang::SourceManager &SM = Context.getSourceManager();
//  clang::FileID MainFileID = SM.getMainFileID();

//  FileID fileId = SM.getFileID(Loc);
  clang::SourceLocation startLoc = SM.getLocForStartOfFile(fileId);

  const clang::RewriteBuffer *RewriteBuf = rewriter.getRewriteBufferFor(fileId);
  if (!RewriteBuf)
    return;


  rewriter.InsertText(startLoc, IncludeStmt_TCTick, true, true);
}

static auto _whileStmtAstNodeKind=ASTNodeKind::getFromNodeKind<clang::WhileStmt>();
static auto _forStmtAstNodeKind=ASTNodeKind::getFromNodeKind<clang::ForStmt>();
static auto _returnStmtAstNodeKind=ASTNodeKind::getFromNodeKind<clang::ReturnStmt>();
static auto _unaryOperatorAstNodeKind=ASTNodeKind::getFromNodeKind<clang::UnaryOperator>();
static auto _implicitCaseExprAstNodeKind=ASTNodeKind::getFromNodeKind<clang::ImplicitCastExpr>();

bool shouldInsert(clang::Stmt *S,ASTNodeKind& parent0NodeKind){
//clang::Stmt::StmtClass & stmtClass
  clang::Stmt::StmtClass stmtClass = S->getStmtClass();

  ////{
  //{内部 不可扩展 的 语法节点 内 是不能插入更多语法结构的 否则语法错误
  //无大括号循环内语句前不要插入， 若要插入，需要先加大括号。
  if(parent0NodeKind.isSame(_forStmtAstNodeKind) || parent0NodeKind.isSame(_whileStmtAstNodeKind)
  //return语句内的语句前不要插入，否则语法错误。
  ||parent0NodeKind.isSame(_returnStmtAstNodeKind)
  //非操作符内的语句前不要插入，否则语法错误。
  ||parent0NodeKind.isSame(_unaryOperatorAstNodeKind)
  //隐式类型转换内的语句前不要插入，否则语法错误。
  ||parent0NodeKind.isSame(_implicitCaseExprAstNodeKind)
  ){
    //如果当前语句S的父亲节点是for语句头，则不插入时钟语句. 单行for语句包含 语句S， 语句S前肯定不能插入，否则 语义不对 甚至 可能语法错误 比如 变量没声明。
    return false;
  }
  ////}

  switch (stmtClass) {//switch开始
    //{不插入时钟语句概率大的情况
    case clang::Stmt::CompoundStmtClass:{
      return false;
    }
    case clang::Stmt::ImplicitCastExprClass:{ //隐式类型转换
      return false;
    }
    case clang::Stmt::StringLiteralClass:{
      return false;
    }
    case clang::Stmt::IntegerLiteralClass:{
      return false;
    }
    case clang::Stmt::DeclRefExprClass:{//存疑,待找到实例以验证
      return false;
    }
    //}

    //{插入时钟语句概率大的情况
    case clang::Stmt::DeclStmtClass:{
      if(parent0NodeKind.isSame(_forStmtAstNodeKind) ){
        //如果当前语句S的父亲节点是for语句头，则不插入时钟语句.
        return false;
      }
      return true;
    }
    case clang::Stmt::CallExprClass:{
      {
      ASTNodeKind compoundStmtAstNodeKind=ASTNodeKind::getFromNodeKind<clang::CompoundStmt>();
      if(parent0NodeKind.isSame(compoundStmtAstNodeKind)){
        //如果调用语句CallExpr的父亲节点是组合语句CompoundStmt，则插入时钟语句. 举例如下:
        // 组合语句CompoundStmt: "{ int age; func1();}" , 其中 "func1();" 是调用语句CallExpr.
        return true;
      }
      }

      {
      ASTNodeKind varDeclAstNodeKind=ASTNodeKind::getFromNodeKind<clang::VarDecl>();
      if(parent0NodeKind.isSame(varDeclAstNodeKind)){
        //如果调用语句CallExpr的父亲节点是变量声明语句VarDecl，则不插入时钟语句. 举例如下:
        // 变量声明语句VarDecl: "float cost=calcCost(3,false);" , 其中 "calcCost(3,false)" 是调用语句CallExpr.
        return false;
      }
      }

      {
      ASTNodeKind binaryOperatorAstNodeKind=ASTNodeKind::getFromNodeKind<clang::BinaryOperator>();
      if(parent0NodeKind.isSame(binaryOperatorAstNodeKind)){
        //如果调用语句CallExpr的父亲节点是 二元操作符BinaryOperator，则不插入时钟语句. 举例如下:
        // 二元操作符BinaryOperator 语句: "name=getName(true);" , 其中 "getName(true)" 是调用语句CallExpr, 二元操作符BinaryOperator 这里是指 赋值号"="
        return false;
      }
      }

      {
      ASTNodeKind ifStmtAstNodeKind=ASTNodeKind::getFromNodeKind<clang::IfStmt>();
      if(parent0NodeKind.isSame(ifStmtAstNodeKind)){
        //如果调用语句CallExpr的父亲节点是 判断语句IfStmt，则不插入时钟语句，否则破坏了原来意思. 举例如下:
        // 判断语句IfStmt  : "if(done) do_job();" , 其中 "do_job()" 是调用语句CallExpr
        return false;
      }
      }

      return true;
    }
    case clang::Stmt::ForStmtClass: {//for循环整体
      return true;
    }
//    case clang::Stmt::UnaryOperatorClass://一元操作符语句,  这里 暂时不插入. 因为需要知道当前是在for循环第3位置 , 还是单独一条语句. for循环第3位置前插入 分割符只能是逗号, 单独一条语句前插入 分割符只能是分号.
//    case clang::Stmt::BinaryOperatorClass://二元操作符语句，暂时不插入。 需要看其内部组成和外部包裹，比如若是被if()包裹 肯定其前肯定不能插入，如果是单独一条语句 其前可以插入。
    case clang::Stmt::IfStmtClass: {//if语句整体
      return true;
    }
    case clang::Stmt::ReturnStmtClass:{
      return true;
    }
    //}
  }//switch结束


  //默认不插入
  return false;
}


FunctionDecl* CodeStyleCheckerVisitor::findFuncDecByName(clang::ASTContext *Ctx,std::string functionName){
//    std::string functionName = "calc";

    TranslationUnitDecl* translationUnitDecl=Ctx->getTranslationUnitDecl();
    for(auto decl:translationUnitDecl->decls()){
      if(FunctionDecl* funcDecl = dyn_cast<FunctionDecl>(decl)){
        if(funcDecl->getNameAsString()==functionName){
          return funcDecl;
        }
      }
    }
    return NULL;
}

/**
 * 获取 给定 位置范围 的源码文本
 * @param sourceRange
 * @param sourceManager
 * @param langOptions
 * @return
 */
std::string CodeStyleCheckerVisitor::getSourceTextBySourceRange(SourceRange sourceRange, SourceManager & sourceManager, const LangOptions & langOptions){
  //ref:  https://stackoverflow.com/questions/40596195/pretty-print-statement-to-string-in-clang/40599057#40599057
//  SourceRange sourceRange=S->getSourceRange();
  CharSourceRange charSourceRange=CharSourceRange::getCharRange(sourceRange);
  llvm::StringRef strRefSourceText=Lexer::getSourceText(charSourceRange, sourceManager, langOptions);

  std::string strSourceText=strRefSourceText.str();
  return strSourceText;
}

/**
 * 获取语句所属源文件路径
 */
bool CodeStyleCheckerVisitor::getSourceFilePathOfStmt(const Stmt *S, const SourceManager &SM,StringRef& fn) {
  SourceLocation Loc = S->getBeginLoc();
  CodeStyleCheckerVisitor::getSourceFilePathAtLoc(Loc,SM,fn);
}

/**
 * 获取位置所属源文件路径
 * 获取语句所属源文件路径
 * code by chatgpt on : https://chat.chatgptdemo.net/
 * @param S
 * @param SM
 * @param fn
 * @return
 */
bool CodeStyleCheckerVisitor::getSourceFilePathAtLoc(SourceLocation Loc, const SourceManager &SM,StringRef& fn) {
//  SourceLocation Loc = S->getBeginLoc();
  if (Loc.isValid()) {
    FileID File = SM.getFileID(Loc);
    const FileEntry *FE = SM.getFileEntryForID(File);
    if (FE) {
      fn=FE->getName();
//      llvm::outs() << "Source File Path: " << FE->getName() << "\n";
      return true;
    }
  }
  return false;
}

/**给定源文件路径是否系统源文件
 * 系统源文件路径举例：
/usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/bits/cpp_type_traits.h
/usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/ext/type_traits.h
/usr/include/x86_64-linux-gnu/bits/iscanonical.h

/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib/clang/15.0.0/include/uintrintrin.h
 * @param fn
 * @return
 */
bool isInternalSysSourceFile(StringRef fn) {
  bool startWithUsr=fn.startswith("/usr/");
  bool isLLVM01=fn.startswith("/app/llvm_release_home/clang+llvm");
  bool isLLVM02=fn.startswith("/llvm_release_home/clang+llvm");
  bool isInternal=(startWithUsr||isLLVM01||isLLVM02);
  return isInternal;
}
void insert_X__t_clock_tick(clang::Rewriter &rewriter, clang::Stmt * stmt, int stackVarAllocCnt,int stackVarFreeCnt,int heapObjAllocCnt,int heapObjcFreeCnt){
  char cStr_X__t_clock_tick[256];

  //X__t_clock_tick(int stackVarAllocCnt, int stackVarFreeCnt, int heapObjAllocCnt, int heapObjcFreeCnt)
  sprintf(cStr_X__t_clock_tick, "%s(%d, %d, %d, %d);\n", CodeStyleCheckerVisitor::funcName_TCTick.c_str(),stackVarAllocCnt,stackVarFreeCnt,heapObjAllocCnt,heapObjcFreeCnt);//"X__t_clock_tick(%d, %d, %d, %d)"
  llvm::StringRef strRef_X__t_clock_tick(cStr_X__t_clock_tick);

//  mRewriter.InsertTextAfter(S->getEndLoc(),"/**/");
  rewriter.InsertTextBefore(stmt->getBeginLoc(),strRef_X__t_clock_tick);

}
/**遍历语句
 *
 * @param stmt
 * @return
 */
bool CodeStyleCheckerVisitor::VisitStmt(clang::Stmt *stmt){

  SourceManager & SM = mRewriter.getSourceMgr();
  const LangOptions & langOpts = mRewriter.getLangOpts();


  clang::SourceLocation beginLoc=stmt->getBeginLoc();
  clang::SourceRange sourceRange=stmt->getSourceRange();
  FileID fileId = SM.getFileID(beginLoc);

  clang::FileID mainFileId = SM.getMainFileID();

  std::string stmtFileAndRange=sourceRange.printToString(SM);

  //获取当前语句S的源码文本
  std::string stmtSourceText=getSourceTextBySourceRange(stmt->getSourceRange(), SM, langOpts);

  Stmt::StmtClass stmtClass = stmt->getStmtClass();
  const char* stmtClassName = stmt->getStmtClassName();


//  std::cout << "[#" << stmtSourceText << "#]:{#" << stmtClassName << "#}" ;  //开发用打印

  //{开发用，条件断点
//  bool shouldBreakPointer=stmtSourceText=="f111(";
//  bool shouldBreakPointer2=stmtSourceText=="!f111(";
  //}

  clang::DynTypedNodeList parentS=this->Ctx->getParents(*stmt);
  size_t parentSSize=parentS.size();
  if(parentSSize>1){
    std::cout << "注意:父节点个数大于1, 为:"<<  parentSSize << "在文件位置:" << stmtFileAndRange  << ",语句是:" << stmtSourceText << std::endl;
  }
  if(parentSSize<=0){
    return true;
  }
  ASTNodeKind parent0NodeKind=parentS[0].getNodeKind();


//    std::cout << parent0NodeKind.asStringRef().str() << std::endl;  //开发用打印
  StringRef fn;
  CodeStyleCheckerVisitor::getSourceFilePathOfStmt(stmt, SM, fn);
  std::string fnStr=fn.str();

  bool _isInternalSysSourceFile  = isInternalSysSourceFile(fn);
  bool _shouldInsert=shouldInsert(stmt, parent0NodeKind);
//  std::cout <<  stmtFileAndRange <<",fileId:" << fileId.getHashValue() << ",mainFileId:" << mainFileId.getHashValue() << ","<< fnStr << ",_isInternalSysSourceFile:" << _isInternalSysSourceFile << ",_shouldInsert:" << _shouldInsert<< std::endl;  //开发用打印

  if( ( !_isInternalSysSourceFile ) && _shouldInsert){

    int stackVarAllocCnt=0;
    int stackVarFreeCnt=0;
    int heapObjAllocCnt=0;
    int heapObjcFreeCnt=0;
    insert_X__t_clock_tick(mRewriter, stmt, stackVarAllocCnt, stackVarFreeCnt, heapObjAllocCnt, heapObjcFreeCnt);

    std::cout<< "在文件位置:" << stmtFileAndRange << ",语句" << stmtSourceText << "前插入时钟语句" <<std::endl;

    if(fileInsertedIncludeStmt.count(fileId)==0){
      CodeStyleCheckerVisitor::insertIncludeToFileStartByLoc(beginLoc, SM, mRewriter);
      std::cout<< "插入'包含时钟'语句到文件头部:" << fnStr <<std::endl;
      fileInsertedIncludeStmt.insert(fileId);
    }
  }else{
//    std::cout<< "not insert X__t_clock_tick to __fn:" << fn.str() <<std::endl;
  }
  return true;
}



bool CodeStyleCheckerVisitor::VisitCXXRecordDecl(CXXRecordDecl *Decl) {
  return true;
}

bool CodeStyleCheckerVisitor::VisitFunctionDecl(FunctionDecl *Decl) {
  return true;
}

bool CodeStyleCheckerVisitor::VisitVarDecl(VarDecl *Decl) {
  return true;
}

bool CodeStyleCheckerVisitor::VisitFieldDecl(FieldDecl *Decl) {

  return true;
}