// Created by z on 2024/6/9.
// [术语] _devOnly == development only == 仅仅方便开发人员使用的函数
#ifndef VarTypeDesc_H
#define VarTypeDesc_H

#include <clang/AST/Type.h>
#include <clang/Basic/IdentifierTable.h>

class VarTypeDesc {
public:
  bool isLambdaType=false;

  bool isBuiltinType=false;
  bool isArrayType=false;
  bool isFunctionType=false;
  bool isPointerType=false;

  bool isDeducedType=false;
  bool isAutoType=false;
  bool isDeducedTemplateSpecializationType=false;

  bool isTypedefType;

  std::string varName;

  std::string msg;

public:
  VarTypeDesc(clang::QualType qualType);
  void fillVarName_devOnly(clang::IdentifierInfo * _varName_);
  void printMsg_devOnly();
};


#endif //VarTypeDesc_H
