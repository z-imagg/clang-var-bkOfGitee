// Created by z on 2024/6/9.

#include <clang/AST/Type.h>
#include <clang/AST/DeclCXX.h>
#include <fmt/core.h>
#include <iostream>
#include "Var/VarTypeDesc.h"

VarTypeDesc::VarTypeDesc(clang::QualType qualType){

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
  bool isBuiltinType = qualType->isBuiltinType();
  const char *typeClassName = typePtr->getTypeClassName();
  std::string typeName = qualType.getAsString();

  std::string  msg=fmt::format("typeName='{}',typeClass={},typeClassName={},isBuiltinType={}\n", typeName, (int)typeClass, typeClassName, isBuiltinType);
  std::cout<<msg;
}

//类型必须穿透typedef链条到达其最终类型
//不关注 auto lambda
//不关注 基本类型
//不关注 指针类型
//不关注 auto 基本类型、指针类型、lambda类型
//关注 auto 似结构体类型
//关注 似结构体类型