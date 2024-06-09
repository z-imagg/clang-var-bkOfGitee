// Created by z on 2024/6/9.

#include <clang/AST/Type.h>
#include <clang/AST/DeclCXX.h>
#include <fmt/core.h>
#include <iostream>
#include "Var/VarTypeFlag.h"
#include "base/UtilTraverseTypeDefChain.h"

VarTypeFlag::VarTypeFlag(clang::QualType qualType){

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


}


VarTypeDescPair::VarTypeDescPair(clang::QualType qualType_origin)
{
  //原类型==typedef第一层别名
  this->qualType_origin=qualType_origin;
  varTypeFlag_origin=VarTypeFlag(qualType_origin);

  //typedef真实类型==typedef叶子类型
  qualType_leaf=UtilTraverseTypeDefChain::traverseTypedefChain(qualType_origin);
  varTypeFlag_leaf=VarTypeFlag(qualType_leaf);

  const clang::Type* typePtr_origin = qualType_origin.getTypePtr();

  clang::Type::TypeClass typeClass_origin = qualType_origin->getTypeClass();
  const char *typeClassName_origin = typePtr_origin->getTypeClassName();
  std::string typeName_origin = qualType_origin.getAsString();

  std::string msg_origin=fmt::format(
    "[origin] typeClassName={},typeClass={},typeName='{}', isLambdaType={},isBuiltinType={},isArrayType={},isFunctionType={},isPointerType={},isDeducedType={},isAutoType={},isDeducedTemplateSpecializationType={},isTypedefType={}\n",
    typeClassName_origin, (int)typeClass_origin, typeName_origin, varTypeFlag_origin.isLambdaType, varTypeFlag_origin.isBuiltinType, varTypeFlag_origin.isArrayType, varTypeFlag_origin.isFunctionType, varTypeFlag_origin.isPointerType, varTypeFlag_origin.isDeducedType, varTypeFlag_origin.isAutoType, varTypeFlag_origin.isDeducedTemplateSpecializationType, varTypeFlag_origin.isTypedefType);

  const clang::Type* typePtr_leaf = qualType_leaf.getTypePtr();

  clang::Type::TypeClass typeClass_leaf = qualType_leaf->getTypeClass();
  const char *typeClassName_leaf = typePtr_leaf->getTypeClassName();
  std::string typeName_leaf = qualType_leaf.getAsString();

  std::string msg_leaf=fmt::format(
    "[leaf] typeClassName={},typeClass={},typeName='{}', isLambdaType={},isBuiltinType={},isArrayType={},isFunctionType={},isPointerType={},isDeducedType={},isAutoType={},isDeducedTemplateSpecializationType={},isTypedefType={}\n",
    typeClassName_leaf, (int)typeClass_leaf, typeName_leaf, varTypeFlag_leaf.isLambdaType, varTypeFlag_leaf.isBuiltinType, varTypeFlag_leaf.isArrayType, varTypeFlag_leaf.isFunctionType, varTypeFlag_leaf.isPointerType, varTypeFlag_leaf.isDeducedType, varTypeFlag_leaf.isAutoType, varTypeFlag_leaf.isDeducedTemplateSpecializationType, varTypeFlag_leaf.isTypedefType);

  this->msg=fmt::format("{} {}", msg_origin, msg_leaf);

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


