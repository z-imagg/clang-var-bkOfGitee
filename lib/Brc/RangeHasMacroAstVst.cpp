#include "Brc/RangeHasMacroAstVst.h"



bool RangeHasMacroAstVst::VisitStmt(clang::Stmt *stmt) {

  SourceManager &SM = CI.getSourceManager();
  ASTContext &Ctx = CI.getASTContext();

  //region 若 当前stmt 是 caseK的子语句， 则累加子语句个数， 否则直接返回。若是子语句且是变量声明语句，则累加变量声明语句个数。
  // 通过 起至范围 包含，判定  当前语句stmt 是否 caseK的子语句
  // 如果遍历到的语句stmt的起至范围 不完全 含在 caseK起至范围内 ， 即不是 caseK的子语句 ，则 不处理，直接返回。
  // 这种情况可能是拿到了一个更大的非终结符号。
  //注意: SourceRange::fullyContains 结果是错误的, 才有自制方法Util::fullContains
  bool inCaseKRange =  Util::fullContains(SM, caseKSrcRange, stmt->getSourceRange());
  if(inCaseKRange) {
    //若 当前stmt 是 caseK的子语句
    //此if块内代码，是针对caseK中每个子语句都会执行的。

    //若 当前stmt 是 caseK的子语句， 则累加子语句个数
    caseKSubStmtCnt++;

    //若当前语句是直接写在'case'内的变量声明   ，则累其语句个数
    if (stmt->getStmtClass() == Stmt::StmtClass::DeclStmtClass) {
      const Stmt* parentStmt;
      Stmt::StmtClass parentStmtCls;
      const Stmt* pParentStmt;
      Stmt::StmtClass pParentStmtCls;
      bool only1P=Util::only1ParentStmtClass(Ctx,stmt,parentStmt,parentStmtCls);
      bool belongSwitchDirectlyInCaseK=
          //直接写在'case'内, 但是其父亲是switch块的
          only1P
           && parentStmtCls==Stmt::StmtClass::CompoundStmtClass
           && Util::only1ParentStmtClass(Ctx,parentStmt,pParentStmt,pParentStmtCls)
           && pParentStmtCls==Stmt::StmtClass::SwitchStmtClass
           ;
      bool normalDirectlyInCase=
          //直接写在'case'内，其父亲是case语句的
          only1P && parentStmtCls == Stmt::StmtClass::CaseStmtClass;
      if(
          //直接写在'case'内, 但是其父亲是switch块的
          belongSwitchDirectlyInCaseK
          ||
          //直接写在'case'内，其父亲是case语句的
          normalDirectlyInCase
//注意: 直接写的'case {'内的 、 直接写在'case'内  不是一回事，而是差别很大。
      ){
        VarDeclDirectlyInCaseKCnt++;
  Util::printStmt(CI.getASTContext(),CI,"直接写在'case'内","VarDeclDirectlyInCaseKCnt",stmt,true);
      }



    }
  }else{
    //若 当前stmt 不是 caseK的子语句， 则直接返回。
    return true;
  }
  //endregion


//  Util::printStmt(CI.getASTContext(),CI,"caseK的子语句","",stmt,true);

  //到此处， 语句stmt 一定是 caseK的子语句


  //region 若 已经标记 caseK子语句中，则直接返回，且不需要再遍历了。
  // 如果已经确定 给定Switch的限定位置范围内 有宏，则直接返回，且不需要再遍历了
  if(hasMacro){
    //若 已经标记 caseK子语句中，则直接返回，且不需要再遍历了。
    //    返回false 表示 告知上层Traverse*方法 不需要再遍历了?
    return false;
  }
  //endregion


  //region 如果遍历到块语句，不处理，直接返回。因为这里只关心单语句，不关心块语句。
  if (clang::isa<clang::CompoundStmt>(stmt)) {
    return true;
  }
  //endregion

  //到此处 遍历到的语句 一定是 单语句，即 不是 块语句

  //region 若此单语句 在宏中，则 标记 caseK子语句中 有宏，并 直接返回 且不需要再遍历了。
  // 如果遍历到的单语句，开始位置在宏中 或 结束位置在宏中，则 给定Switch的限定位置范围内 有宏，直接返回，且不需要再遍历了。
  SourceLocation B = stmt->getBeginLoc();
  SourceLocation E = stmt->getEndLoc();
  bool inMacro = Util::LocIsInMacro(B,SM) || Util::LocIsInMacro(E,SM);
  if(!hasMacro ){
    if(inMacro){
      //若此单语句 在宏中, 则 标记 caseK子语句中 有宏
      hasMacro=true;

      //并 直接返回 且不需要再遍历了
      return false;
    }
  }
  //endregion

  //其他情况，继续遍历
  return true;
}