// Created by z on 2024/6/9.
// [术语] _devOnly == development only == 仅仅方便开发人员使用的函数
#ifndef VarTypeDesc_H
#define VarTypeDesc_H

#include <clang/AST/Type.h>
#include <clang/Basic/IdentifierTable.h>
#include "Var/VarTypeFlag.h"



class VarTypeDesc{
  //类型必须穿透typedef链条到达其最终类型
  //  如果不是typedef ，则 qualType_leaf表示的类型 == qualType表示的类型
  //  如果是typedef,   则 qualType==typedef的起点类型==typedef的第一个别名, qualType_leaf==typedef的真实类型==typedef的叶子类型
//  qualType_leaf=UtilTraverseTypeDefChain::traverseTypedefChain(qualType);

public:

  //原始类型存根
  clang::QualType qualType;
  VarTypeFlag varTypeFlag;

  //typedef类型别名链条的叶子 == typedef类型别名对应的真实类型
  clang::QualType qualType_leaf;
  VarTypeFlag varTypeFlag_leaf;


  std::string varName;

  std::string msg;

public:
  VarTypeDesc(clang::QualType qualType);
  void focus(bool& focus_);
  void fillVarName_devOnly(clang::IdentifierInfo * _varName);
  void printMsg_devOnly();
};

#endif //VarTypeDesc_H
