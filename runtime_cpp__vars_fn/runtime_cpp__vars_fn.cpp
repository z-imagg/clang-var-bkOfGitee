
#include "runtime_cpp__vars_fn.h"
#include "rntm_c__TmPnt_ThrLcl.h"

// 本模块runtime_cpp__vars_fn 只允许被C++使用，而不允许被C使用
#ifndef __cplusplus
// 本模块若被C使用, 则报错以终止编译
#error "[ERROR] runtime_cpp__vars_fn must be used as C++ not C @runtime_cpp__vars_fn.cpp"
#else
// 本模块必须被C++使用

////runtime_cpp 正文开始
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
      std::cout << jsonLineTxt; //运行 /app2/jdk-jdk-24-0/build_home/jdk/bin/javac 漰溃, 详细见 https://prgrmz07.coding.net/p/app/d/jdk/git/tree/054566a94078594ac3e345afe5427446fcb49d13/README.md
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

////runtime_cpp 正文结束



////fridaHelper 正文开始

#include <string>
#include <stdio.h>
#include <string.h>
/** 供给frida用的cxx std::string构造函数 包装器
 *
 * cxx std::string 构造函数 返回类型 理论上 应该是 std::string ， 而 frida 的NativeFunction貌似不支持表达 自定义struct、自定义class
 * 因此只能包装

objdump --syms app.elf | grep fridaHelper
# 00000000000012f1 g     F .text	000000000000002b              _Z40fridaHelper__cxxFuncWrap__std_string_newv
# 000000000000131c g     F .text	000000000000003e              _Z43fridaHelper__cxxFuncWrap__std_string_deletePv
*/
void* fridaHelper__cxxFuncWrap__std_string_new(){
  std::string *ptr= new std::string();
//  printf("[app.cpp, fridaHelper__cxxFuncWrap__std_string_new] ptr=%x\n",ptr);
  return ptr;
}
void fridaHelper__cxxFuncWrap__std_string_delete(void* ptr_CxxStdString){
  std::string* ptr=(std::string*)(ptr_CxxStdString);
//  printf("[app.cpp, fridaHelper__cxxFuncWrap__std_string_delete] ptr=%x\n",ptr);
  delete ptr;
}
int fridaHelper__cxxFuncWrap__std_string_size(void* ptr_CxxStdString){
  std::string* ptr=(std::string*)(ptr_CxxStdString);
  int size_CxxStdString=ptr->size();
//  printf("[app.cpp, fridaHelper__cxxFuncWrap__std_string_size] size_CxxStdString=%d\n",size_CxxStdString);
  return size_CxxStdString;
}

int fridaHelper__cxxFuncWrap__std_string_cstr(void* ptr_CxxStdString, int cStrOutLen, char * cStrOut_){
// CxxFnOutArg_stdString__Fn06.stdStr_2_fridaBuf__Err1==1
#define _Err1 1
// CxxFnOutArg_stdString__Fn06.stdStr_2_fridaBuf__OK==0
#define _OK 0
// CxxFnOutArg_stdString__Fn06.stdStr_2_fridaBuf_gap==11
#define _stdStr_2_fridaBuf_gap 11

  std::string* ptr=(std::string*)(ptr_CxxStdString);
  int size_CxxStdString=ptr->size();
  if(size_CxxStdString+_stdStr_2_fridaBuf_gap<cStrOutLen){
    return _Err1;
  }
  const char* cstr=ptr->c_str();
  strncpy(cStrOut_,cstr, size_CxxStdString);
  return _OK;
}

////fridaHelper 正文结束
#endif  //__cplusplus判断结束