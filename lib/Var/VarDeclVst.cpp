#include "Var/VarDeclVst.h"
#include "Var/VarTypeDesc.h"

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "base/Util.h"
#include "base/ASTContextUtil.h"
#include "base/StrUtil.h"
#include "Var/RangeHasMacroAstVst.h"
#include "Var/CollectIncMacro_PPCb.h"
#include <vector>

#include <fmt/core.h>
#include <iostream>

#include "base/MyAssert.h"
#include "Var/Constant.h"
#include "base/UtilParentKind.h"
#include "base/UtilMainFile.h"


using namespace llvm;
using namespace clang;


//结构体变量声明末尾 插入 'createVar__RtCxx(_varLs_ptr,"变量类型名",变量个数);'
bool VarDeclVst::insertAfter_VarDecl( bool useCXX,std::vector<const VarTypeDesc*>& varTypeDescVec,LocId varDeclLocId, SourceLocation varDeclEndLoc ){
  //const std::string typeName,int varCnt
  std::string funcName=Constant::fnNameS__createVar[useCXX];
    std::vector<std::string> code_ls;
    //  构造插入语句
    std::transform(varTypeDescVec.begin(), varTypeDescVec.end(),
         std::back_inserter(code_ls),
         [&funcName,&varDeclLocId](const VarTypeDesc* varTypeDescK){
      std::string code_inserted=fmt::format(
        "{}(_vdLs, \"{}\", {})  /* 创建变量通知,  {} */ ;", // createVar__RtCxx 或 createVar__RtC00
        funcName,
        varTypeDescK->typeName, 1, //要求createVar__RtC 增加变量尺寸字段
        varDeclLocId.to_string()
      );
      return code_inserted;
    });

    //用 空格 join code_ls 为 单个大字符串
  std::ostringstream oss;
  std::copy(code_ls.begin(), code_ls.end(), std::ostream_iterator<std::string>(oss, "  "));
  std::string code_ls_txt = oss.str();

    bool insertResult=mRewriter_ptr->InsertTextAfterToken(varDeclEndLoc , code_ls_txt);


    //记录已插入语句的节点ID们以防重： 即使重复遍历了 但不会重复插入
  //用funcEnterLocIdSet的尺寸作为LocationId的计数器
    VarDeclLocIdSet.insert(varDeclLocId);

    return insertResult;
}

bool VarDeclVst::TraverseDeclStmt(DeclStmt* declStmt){

    //获取主文件ID,文件路径
    FileID mainFileId;
    std::string filePath;
  UtilMainFile::getMainFileIDMainFilePath(SM,mainFileId,filePath);

    const SourceLocation declStmtBgnLoc = declStmt->getEndLoc();

    //忽略for中的循环变量声明, 因该位置插入函数调用语句createVar__RtC**不符合语法
    //忽略'for(T var; var<3; var++) { T var2; }' 中的循环变量声明'T var', 该声明的特征是其父亲是ForStmt
    //   请注意 'T var2;' 的父亲是CompoundStmt
    DynTypedNode parent;
    ASTNodeKind parentNK;
    bool only1P = UtilParentKind::only1ParentNodeKind(CI, *Ctx, declStmt, parent, parentNK);
    assert(only1P);
    bool parentNKIsForStmt = ASTNodeKind::getFromNodeKind<ForStmt>().isSame(parentNK);
    if(parentNKIsForStmt){
        return false;
    }
    //TODO ForEach 要像 ForStmt 一样 处理么?


    Decl *singleDecl;

  std::vector<const Decl*> declVec;

  bool isSingleDecl = declStmt->isSingleDecl();
  if(isSingleDecl){
    //单声明（单变量声明、单函数声明、单x声明）
    singleDecl = declStmt->getSingleDecl();
    declVec.push_back(singleDecl);
  }else{
    //多声明（多变量声明、多函数声明、多x声明）
    const DeclGroupRef &dg = declStmt->getDeclGroup();
    std::copy(dg.begin(), dg.end(), std::back_inserter(declVec));

  }

  std::vector<const VarTypeDesc*> vTDPtrVec;
  int varCnt=std::distance(declVec.begin(),declVec.end());
  //VarTypeDesc对象们拥有者为vTDVec
  std::vector<VarTypeDesc> vTDVec(varCnt, VarTypeDesc());
  //遍历每一个声明
  for(int k=0; k < declVec.size(); k++){
    const Decl* decl_k=declVec.at(k);
    VarTypeDesc & varTypeDesc_k  = vTDVec.at(k);
    this->process_singleDecl(decl_k, varTypeDesc_k/*出量*/ );
    //第k个声明，若是似结构体则记数
    if(varTypeDesc_k.focus){
      vTDPtrVec.push_back(&varTypeDesc_k);
    }

  }

  //  Ctx.langOpts.CPlusPlus 估计只能表示当前是用clang++编译的、还是clang编译的, [TODO] 但不能涵盖 'extern c'、'extern c++'造成的语言变化
    bool useCxx=ASTContextUtil::useCxx(Ctx);
    //是结构体
    if(!vTDPtrVec.empty()){
        //按照左右花括号，构建位置id，防止重复插入
        //  在变量声明语句这，不知道如何获得当前所在函数名 因此暂时函数名传递空字符串
        LocId declStmtBgnLocId=LocId::buildFor(filePath,declStmtBgnLoc, SM);
        //【执行业务内容】 向threadLocal记录发生一次 :  栈区变量声明 其类型为typeClassName
        //只有似结构体变量才会产生通知
       insertAfter_VarDecl(useCxx, vTDPtrVec, declStmtBgnLocId, declStmtBgnLoc);
       //不要返回false, 否则导致clang外层遍历器不再遍历后边的变量声明们
    }

    return true;
}

// 递归遍历typedef链条
//clang::Type* actualType = traverseTypedefChain(valueDecl->getType());
const clang::Type* traverseTypedefChain(clang::QualType qualType) {
  const clang::Type* typePtr = qualType.getTypePtr();
  clang::TypedefNameDecl* typedefDecl = nullptr;

  // 如果是typedef，获取typedef的目标类型
  if (const clang::TypedefType* typedefType = llvm::dyn_cast<clang::TypedefType>(typePtr)) {
    typedefDecl = typedefType->getDecl();

    // 从TypedefDecl中获取目标类型
    clang::QualType typedef_to = typedefDecl->getUnderlyingType();
    if(typedef_to->isBuiltinType()){

    }

    const std::string typedef_fromName = typedefDecl->getNameAsString();
    const std::string typedef_toName = typedef_to.getAsString();
    std::string msg=fmt::format("typedef_fromName={}, typedef_toName={}\n", typedef_fromName, typedef_toName);
    std::cout<<msg;

    // 递归处理目标类型
    return traverseTypedefChain(typedef_to);
  }

  // 对于其他类型，您可能需要添加额外的处理逻辑，例如指针、数组等

  // 假设当前类型不是typedef，则返回它自己
  return typePtr;
}


bool VarDeclVst::process_singleDecl(const Decl *singleDecl,VarTypeDesc& varTypeDesc_){
  //  QualType &qualType_, bool& focus_, std::string &typeName_, std::string &varName_
    if (const ValueDecl *valueDecl = dyn_cast_or_null<ValueDecl>(singleDecl)) {
      QualType qualType = valueDecl->getType();

//      typeName_ = qualType.getAsString();
      VarTypeDesc::build(qualType,varTypeDesc_);

//      VarTypeDesc varTypeDesc(qualType_);
      varTypeDesc_.fillVarName_devOnly(valueDecl->getIdentifier());
      varTypeDesc_.printMsg_devOnly();
//      varTypeDesc.focus(focus_);
//      varName_=varTypeDesc.varName;

//        std::cout<<fmt::format("[返回]likeStruct=={}\n", focus_);
    }

    return true;
}

/**  clang::Type::Auto == typeClass , 但是typeName不同 , typeName为 'class (lambda at ...)' 或 typeName='class ...'
tag1,title1,parent0NodeKind:CompoundStmt,nodeID:2305,文件路径、坐标:</fridaAnlzAp/clp-zz/test_in/test_main.cpp:31:9, line:36:10>,getStmtClassName:DeclStmt,getStmtClass:12,mainFileId:1,fileId:1,源码:【auto fn_point = [](const Point& point) {
            if(point.x>point.y)
                return point.x+point.y;
            else
                return 0.1;
        }】
typeName='class (lambda at /fridaAnlzAp/clp-zz/test_in/test_main.cpp:31:25)',typeClass=14,typeClassName=Auto,isBuiltinType=false
qualTypeAsStr=class (lambda at /fridaAnlzAp/clp-zz/test_in/test_main.cpp:31:25)
[返回]likeStruct==true

tag1,title1,parent0NodeKind:CompoundStmt,nodeID:2483,文件路径、坐标:</fridaAnlzAp/clp-zz/test_in/test_main.cpp:42:9, col:42>,getStmtClassName:DeclStmt,getStmtClass:12,mainFileId:1,fileId:1,源码:【auto user_auto_var = UserEntity()】
typeName='class UserEntity',typeClass=14,typeClassName=Auto,isBuiltinType=false
qualTypeAsStr=class UserEntity


*/