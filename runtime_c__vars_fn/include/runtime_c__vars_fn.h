
#ifndef runtime_c__vars_fn_H
#define runtime_c__vars_fn_H

#include "list.h" //#include <vector>
#include "sds.h" // #include <string>

struct __VarDecl{


    //业务字段
    sds varTypeName;
    int varCnt;
};
typedef struct __VarDecl _VarDecl;

struct __VarDeclLs{
    //定位字段
    sds srcFilePath;
    sds funcName;
    int funcLBrc_line;
    int funcLBrc_column;

    //业务字段
    list_t* _vdVec; // std::vector<_VarDecl> * _vdVec;
};
typedef struct __VarDeclLs _VarDeclLs;

//   函数左花括号后 插入 'void* _varLs_ptr=_init_varLs_inFn();'
_VarDeclLs *  _init_varLs_inFn(sds srcFilePath, sds funcName,int funcLBrc_line,int funcLBrc_column);


//结构体变量声明末尾 插入 'createVar(_varLs_ptr,"变量类型名",变量个数);'
void createVar(_VarDeclLs * _vdLs, sds varTypeName,int varCnt);


//【销毁变量通知】    函数右花括号前 插入 'destroyVarLs_inFn(_varLs_ptr);'
void destroyVarLs_inFn(_VarDeclLs * _vdLs);
#endif //runtime_c__vars_fn_H