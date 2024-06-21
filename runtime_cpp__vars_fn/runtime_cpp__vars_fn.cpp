
#include "runtime_cpp__vars_fn.h"
#include "rntm_c__TmPnt_ThrLcl.h"

// 本模块runtime_cpp__vars_fn 只允许被C++使用，而不允许被C使用
#ifndef __cplusplus
// 本模块若被C使用, 则报错以终止编译
#error "[ERROR] runtime_cpp__vars_fn must be used as C++ not C @runtime_cpp__vars_fn.cpp"
#else
// 本模块必须被C++使用

#include <iostream>
#include <algorithm>
#include <numeric>
#include <sstream>

//函数左花括号后 插入 'void* _varLs_ptr=_init_varLs_inFn__RtCxx();'
_VarDeclLs *  _init_varLs_inFn__RtCxx(std::string srcFilePath, std::string funcName, int funcLBrc_line, int funcLBrc_column){
    //new:HeapObj_vdLs
    _VarDeclLs *vdLs = new _VarDeclLs;
    //new:HeapObj_vdVec
    vdLs->_vdVec=new std::vector<_VarDecl>();

    vdLs->srcFilePath=srcFilePath;
    vdLs->funcName=funcName;
    vdLs->funcLBrc_line=funcLBrc_line;
    vdLs->funcLBrc_column=funcLBrc_column;

    //依赖模块 runtime_c__TmPnt_ThreadLocal
    vdLs->TmPnt=TL_TmPnt__get();

    return vdLs;
}


//结构体变量声明末尾 插入 'createVar__RtCxx(_varLs_ptr,"变量类型名",变量个数);'
void createVar__RtCxx(_VarDeclLs *_vdLs, std::string varTypeName,  int varSize,bool varIsArr,int arrEleSize){

    _VarDecl vd;
    vd.varTypeName=varTypeName;
    vd.varSize=varSize;
    vd.varIsArr=varIsArr;
    vd.arrEleSize=arrEleSize;

    _vdLs->_vdVec->push_back(vd);

}


#define _jsonItem( fieldName, fieldVal)      << "\"" << fieldName << "\":\"" << fieldVal
#define jsonItemEnd( fieldName, fieldVal)                 _jsonItem( fieldName, fieldVal) << "\""
#define jsonItem( fieldName, fieldVal)                    _jsonItem( fieldName, fieldVal)  << "\","

//日志开头标记
//  以换行开头的理由是，避开应用程序日志中不换行的日志 造成的干扰。
const static std::string _LogLinePrefix="\n__@__@";

//【销毁变量通知】 函数右花括号前 插入 'destroyVarLs_inFn__RtCxx(_varLs_ptr);'
void destroyVarLs_inFn__RtCxx(_VarDeclLs *_vdLs, std::string * jsonTxtOut_){
    std::vector<_VarDecl> *_vdVec = _vdLs->_vdVec;

  long varDeclCnt = std::distance(_vdVec->begin(), _vdVec->end());

if(varDeclCnt > 0){
    _VarDecl zero; zero.varSize=0;

    //循环累加获得变量个数
    const _VarDecl &sum_vd = std::accumulate(
    _vdVec->begin(),
    _vdVec->end(),
    zero,
[](const _VarDecl &a, const _VarDecl &b) {
      _VarDecl result;
      result.varSize = a.varSize + b.varSize;
      return result;
    }
    );

    int varSizeSum=sum_vd.varSize;

    //组装_vdLs为json并输出到控制台
    std::stringstream ss;
    ss << _LogLinePrefix
        jsonItem("srcFilePath",_vdLs->srcFilePath)
        jsonItem("funcLBrc_line",_vdLs->funcLBrc_line)
        jsonItem("funcLBrc_column",_vdLs->funcLBrc_column)
        jsonItem("varDeclCnt",varDeclCnt)
        jsonItem("varSizeSum",varSizeSum)
    ;
    ss << "[";
    //循环 变量们 ， 将 单变量 转换 json文本
    std::for_each(_vdVec->begin(), _vdVec->end(), [&ss](const _VarDecl vd){
      ss
          jsonItem("kind","runtimeCxx")
          jsonItem("varTypeName",vd.varTypeName)
          jsonItem("varSize",vd.varSize)
          jsonItem("varIsArr",vd.varIsArr)
          jsonItemEnd("arrEleSize",vd.arrEleSize)
      ;
    });
    ss << "]\n";
    std::string jsonLineTxt=ss.str();

    // 如果调用者不接收json串，则打印到控制台
    if(jsonTxtOut_== nullptr){
      std::cout << jsonLineTxt;
    }
    //否则[如果jsonTxtOut_非空] ，则正常返回json文本
    else{
      //复制jsonLineTxt到jsonTxtOut_
      jsonTxtOut_->assign(jsonLineTxt );
    }
    
    //}

  }

    //delete:HeapObj_vdVec
    delete _vdLs->_vdVec; _vdLs->_vdVec= nullptr;
    //delete:HeapObj_vdLs
    delete _vdLs;

}


#endif  //__cplusplus判断结束