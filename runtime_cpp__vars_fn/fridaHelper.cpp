

// 本模块runtime_cpp__vars_fn 只允许被C++使用，而不允许被C使用
#ifndef __cplusplus
// 本模块若被C使用, 则报错以终止编译
#error "[ERROR] runtime_cpp__vars_fn must be used as C++ not C @runtime_cpp__vars_fn.cpp"
#else
// 本模块必须被C++使用

////此文件正文开始

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

////此文件正文结束

#endif  //__cplusplus判断结束