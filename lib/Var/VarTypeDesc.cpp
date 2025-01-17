// Created by z on 2024/6/9.

#include <clang/AST/Type.h>
#include <clang/AST/DeclCXX.h>
#include <fmt/core.h>
#include <iostream>
#include "Var/VarTypeFlag.h"
#include "Var/VarTypeDesc.h"
#include "base/UtilTraverseTypeDefChain.h"

// 计算 是否关注 该函数
static bool _calc_focus(VarTypeFlag vT_Leaf , clang::QualType qT_Leaf){
// 其实只有 isLambdaType 是需要VarTypeFlag去特定计算的,其余的不需要
  bool focus=
(
// 非内置类型
  (!qT_Leaf->isBuiltinType()) &&
// 非枚举类型
  (!qT_Leaf->isEnumeralType()) &&
// 非lambda类型
  (!vT_Leaf.isLambdaType)
) || (
// 指针类型
  qT_Leaf->isPointerType() ||
// 指针类型
  qT_Leaf->isReferenceType() ||
// 记录类型
  qT_Leaf->isRecordType() ||
// elaborated类型
  qT_Leaf->isElaboratedTypeSpecifier() ||
// 数组类型
  qT_Leaf->isArrayType() ||
// 类类型
  qT_Leaf->isClassType() ||
// 结构体or类类型
  qT_Leaf->isStructureOrClassType() ||
// 联合类型
  qT_Leaf->isUnionType()
);

  return focus;
}

void VarTypeDesc::build(clang::QualType _qualType, VarTypeDesc& self/*出量*/){

//}
//VarTypeDesc::VarTypeDesc(clang::QualType _qualType)
//{
  std::string msg_origin;

  //原类型==typedef第一层别名
  self.qualType=_qualType;
  self.varTypeFlag=VarTypeFlag(_qualType, msg_origin/*出量*/);

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

    self.msg=fmt::format(" [origin] {}  [leaf] {}",  msg_origin, msg_leaf);
  }else{
    self.msg=fmt::format(" [origin] {}   ",  msg_origin );
  }


  //////////计算focus
  VarTypeFlag vT_Leaf = self.getVarTypeFlag_Leaf();
  clang::QualType qT_Leaf = self.getQualType_Leaf();
  assert(vT_Leaf._inited);
  self.typeName = qT_Leaf.getAsString();
  self.msg=fmt::format("typeName={}  {}", self.typeName, self.msg );
// 其实只有 isLambdaType 是需要VarTypeFlag去特定计算的,其余的不需要
// 计算 是否关注 该函数
  self.focus=_calc_focus(vT_Leaf, qT_Leaf );

//不关注 auto lambda
//不关注 基本类型
//不关注 指针类型
//不关注 auto 基本类型、指针类型、lambda类型
//关注 auto 似结构体类型
//关注 似结构体类型
  //////////

  return; //Release版 函数末尾始终要有return语句
}

void VarTypeDesc::fillVarName_devOnly(clang::IdentifierInfo * _varName){
  this->varName=  _varName->getName();

  this->msg=fmt::format("[VarDecl描述] varName={}, {} \n", this->varName,this->msg);

  return; //Release版 函数末尾始终要有return语句
}


void VarTypeDesc::printMsg_devOnly(){
std::cout<<this->msg;

return; //Release版 函数末尾始终要有return语句
}


