
#include "rntm_c__TmPnt_ThrLcl.h"

// 始终保持 为 C函数，而非C++函数
#ifdef __cplusplus
extern "C" {
#endif


// __thread是gcc扩展 ,
//    __thread == ThreadLocal
//  术语  TL_ == ThreadLocal_
// frida会通过函数TL_TmPnt__update 修改 以下两个threadLocal变量
__thread int TL_curThreadId = -1;
__thread int TL_TmPnt = -1;

void TL_TmPnt__update(int _Tl_curThreadId, int _TmPnt_new) {
    TL_curThreadId = _Tl_curThreadId;
    TL_TmPnt = _TmPnt_new;
}

int TL_TmPnt__get() {
    return TL_TmPnt;
}

//打印ThreadLocal中某变量指针
#include <stdio.h>
void TL_TmPnt__printPtr( ) {
  printf("TL_TmPnt@0x%x\n", &TL_TmPnt) ;
}

#ifdef __cplusplus
};
#endif