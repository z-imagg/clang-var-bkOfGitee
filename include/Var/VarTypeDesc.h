// Created by z on 2024/6/9.

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
  void fillVarName(clang::IdentifierInfo * _varName_);
  void printMsg();
};


#endif //VarTypeDesc_H
