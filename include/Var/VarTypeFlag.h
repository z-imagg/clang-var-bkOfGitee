// Created by z on 2024/6/9.
// [术语] _devOnly == development only == 仅仅方便开发人员使用的函数
#ifndef VarTypeFlag_H
#define VarTypeFlag_H

#include <clang/AST/Type.h>
#include <clang/Basic/IdentifierTable.h>

#define VarTypeFlag_Print_Format " typeClassName={},typeClass={},typeName='{}', isLambdaType={},isBuiltinType={},isArrayType={},isFunctionType={},isPointerType={},isDeducedType={},isAutoType={},isDeducedTemplateSpecializationType={},isTypedefType={}\n"
class VarTypeFlag {
public:
  //原始类型存根
//  clang::QualType qualType;

  //typedef类型别名链条的叶子 == typedef类型别名对应的真实类型
//  clang::QualType qualType_leaf;

  bool isLambdaType=false;

  bool isBuiltinType=false;
  bool isArrayType=false;
  bool isFunctionType=false;
  bool isPointerType=false;

  bool isDeducedType=false;
  bool isAutoType=false;
  bool isDeducedTemplateSpecializationType=false;

  bool isTypedefType;


public:
  VarTypeFlag()=default;
  VarTypeFlag(clang::QualType qualType,std::string & msg_);

};



#endif //VarTypeFlag_H
