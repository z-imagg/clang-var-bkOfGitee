// Created by z on 2024/6/9.

#include <clang/AST/Type.h>
#include <clang/AST/DeclCXX.h>
#include <fmt/core.h>
#include <iostream>
#include "Var/VarTypeFlag.h"
#include "base/UtilTraverseTypeDefChain.h"

VarTypeFlag::VarTypeFlag(clang::QualType qualType,std::string & msg_){

{//不关注 auto lambda
  /* 本代码块判断 变量是否 被赋值为 lambda表达式
   比如以下变量 lambda_func_var_1  是 lambda表达式
   auto lambda_func_var_1=[&k]( ){ return 0.0;};

   但是以下变量 lambda_func_var_2 是 Pointer类型
   double (*lambda_func_var_2)() = []() { return 0.0; };
   */
  clang::CXXRecordDecl *cXXRecordDecl=qualType->getAsCXXRecordDecl();
  if(cXXRecordDecl!= nullptr){
    this->isLambdaType=cXXRecordDecl->isLambda();
  }
}


  const clang::Type* typePtr = qualType.getTypePtr();

  if (const clang::BuiltinType* builtinType = llvm::dyn_cast<clang::BuiltinType>(typePtr)) {
    isBuiltinType=true;
  }
  if (const clang::ArrayType* arrayType = llvm::dyn_cast<clang::ArrayType>(typePtr)) {
    isArrayType=true;
    return;
  }

  if (const clang::FunctionType* functionType = llvm::dyn_cast<clang::FunctionType>(typePtr)) {
    isFunctionType=true;
  }

  if (const clang::PointerType* pointerType = llvm::dyn_cast<clang::PointerType>(typePtr)) {
    isPointerType=true;
  }

  if (const clang::DeducedType* deducedType = llvm::dyn_cast<clang::DeducedType>(typePtr)) {
    isDeducedType=true;
  }
  if (const clang::AutoType* autoType = llvm::dyn_cast<clang::AutoType>(typePtr)) {
    isAutoType=true;
  }
  if (const clang::DeducedTemplateSpecializationType* dtsType = llvm::dyn_cast<clang::DeducedTemplateSpecializationType>(typePtr)) {
    isDeducedTemplateSpecializationType=true;
  }

  if (const clang::TypedefType* typedefType = llvm::dyn_cast<clang::TypedefType>(typePtr)) {
    isTypedefType=true;
  }


  clang::Type::TypeClass typeClass = qualType->getTypeClass();
  const char *typeClassName = typePtr->getTypeClassName();
  std::string typeName = qualType.getAsString();
  msg_=fmt::format(
    VarTypeFlag_Print_Format,
    typeClassName, (int)typeClass, typeName, isLambdaType, isBuiltinType, isArrayType, isFunctionType, isPointerType, isDeducedType, isAutoType, isDeducedTemplateSpecializationType, isTypedefType);

}


VarTypeDescPair::VarTypeDescPair(clang::QualType qualType)
{
  std::string msg_origin;

  //原类型==typedef第一层别名
  this->qualType=qualType;
  varTypeFlag=VarTypeFlag(qualType,msg_origin);

  const clang::Type* typePtr = qualType.getTypePtr();

  //递归遍历typedef链条 直到 typedef指向的真实类型
  //   typedef真实类型==typedef叶子类型
  qualType_leaf=UtilTraverseTypeDefChain::traverseTypedefChain(qualType);

  const clang::Type* typePtr_leaf = qualType_leaf.getTypePtr();
  std::string msg_leaf="";

  // 如果 是typedef类型
  if(varTypeFlag.isTypedefType){
    //断言 '起点类型 不等于 真实类型' ==  '是typedef类型'
    //  如果断言失败 可能要检查 UtilTraverseTypeDefChain::traverseTypedefChain 、 clang::QualType 复制时 不应该复制getTypePtr中的字段 ?
    assert(typePtr != typePtr_leaf);

    varTypeFlag_leaf=VarTypeFlag(qualType_leaf,msg_leaf);

  }

  this->msg=fmt::format("[origin] {}  [leaf] {}", msg_origin, msg_leaf);

}
void VarTypeDescPair::focus(bool& focus_){


//不关注 auto lambda
//不关注 基本类型
//不关注 指针类型
//不关注 auto 基本类型、指针类型、lambda类型
//关注 auto 似结构体类型
//关注 似结构体类型
}

void VarTypeDescPair::fillVarName_devOnly(clang::IdentifierInfo * _varName){
  this->varName=  _varName->getName();

  this->msg=fmt::format("[VarDecl描述] varName={}, {}", this->varName,this->msg);
}


void VarTypeDescPair::printMsg_devOnly(){
std::cout<<this->msg;
}


