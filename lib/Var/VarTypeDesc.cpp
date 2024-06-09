// Created by z on 2024/6/9.

#include <clang/AST/Type.h>
#include <clang/AST/DeclCXX.h>
#include <fmt/core.h>
#include <iostream>
#include "Var/VarTypeFlag.h"
#include "Var/VarTypeDesc.h"
#include "base/UtilTraverseTypeDefChain.h"


void VarTypeDesc::build(clang::QualType _qualType, VarTypeDesc& self){

//}
//VarTypeDesc::VarTypeDesc(clang::QualType _qualType)
//{
  std::string msg_origin;

  //原类型==typedef第一层别名
  self.qualType=_qualType;
  self.varTypeFlag=VarTypeFlag(_qualType, msg_origin);

  const clang::Type* typePtr = _qualType.getTypePtr();

  //递归遍历typedef链条 直到 typedef指向的真实类型
  //   typedef真实类型==typedef叶子类型
  self.qualType_leaf=UtilTraverseTypeDefChain::traverseTypedefChain(_qualType);

  const clang::Type* typePtr_leaf = self.qualType_leaf.getTypePtr();
  std::string msg_leaf="";

  // 如果 是typedef类型
  if(self.varTypeFlag.isTypedefType){
    //断言 '起点类型 不等于 真实类型' ==  '是typedef类型'
    //  如果断言失败 可能要检查 UtilTraverseTypeDefChain::traverseTypedefChain 、 clang::QualType 复制时 不应该复制getTypePtr中的字段 ?
    assert(typePtr != typePtr_leaf);

    self.varTypeFlag_leaf=VarTypeFlag(self.qualType_leaf, msg_leaf);

  }

  self.msg=fmt::format("[origin] {}  [leaf] {}", msg_origin, msg_leaf);

  //////////计算focus
  VarTypeFlag &vTF = self.varTypeFlag.isTypedefType ? self.varTypeFlag_leaf : self.varTypeFlag;
  clang::QualType &qT = self.varTypeFlag.isTypedefType ? self.qualType_leaf : self.qualType;
  assert(vTF._inited);
  self.typeName = qT.getAsString();
//  clang::Type::TypeClass typeClass = qT->getTypeClass();
//  clang::Type::Record == typeClass;
// !vTF.isBuiltinType
// !vTF.isPointerType
// 其实只有 isLambdaType 是需要VarTypeFlag去特定计算的,其余的不需要
  self.focus= ((!qT->isBuiltinType()) && (!qT->isPointerType()) && (!vTF.isLambdaType)  ) || (qT->isRecordType() || qT->isElaboratedTypeSpecifier() || qT->isArrayType() || qT->isClassType() || qT->isStructureOrClassType() || qT->isUnionType() );

//不关注 auto lambda
//不关注 基本类型
//不关注 指针类型
//不关注 auto 基本类型、指针类型、lambda类型
//关注 auto 似结构体类型
//关注 似结构体类型
  //////////
}

void VarTypeDesc::fillVarName_devOnly(clang::IdentifierInfo * _varName){
  this->varName=  _varName->getName();

  this->msg=fmt::format("[VarDecl描述] varName={}, {}", this->varName,this->msg);
}


void VarTypeDesc::printMsg_devOnly(){
std::cout<<this->msg;
}


