#pragma message("VarBE_inserted")
#include "MyClz.h"
#include <algorithm>
#include <list>

void funcEatLambdaFunc01(my_nsp::MyClass *(*func01)(my_nsp::UserEntity *)){_VarDeclLs * _vdLs=_init_varLs_inFn__RtCxx("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "funcEatLambdaFunc01", 5, 75); /* 初始化函数变量列表, */
  my_nsp::ScoreRelation sR1;createVar__RtCxx(_vdLs, "my_nsp::ScoreRelation", 1)  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:6,28 */ ;
  func01(nullptr);
  destroyVarLs_inFn__RtCxx(_vdLs); /* 销毁函数变量列表: */return;
}

int main(int argc, char** argv){_VarDeclLs * _vdLs=_init_varLs_inFn__RtCxx("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "main", 11, 32); /* 初始化函数变量列表, */
  my_nsp::MyClass varMyClass;createVar__RtCxx(_vdLs, "my_nsp::MyClass", 1)  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:12,29 */ ;
  static Point pnt1;createVar__RtCxx(_vdLs, "struct Point", 1)  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:13,20 */ ;
  struct Point pnt2;createVar__RtCxx(_vdLs, "struct Point", 1)  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:14,20 */ ;
  {
    struct Point * ptr1=&pnt1;
    struct Point * ptr2=&pnt2;
    struct Point pnt3;createVar__RtCxx(_vdLs, "struct Point", 1)  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:18,22 */ ;
  }


  std::list<my_nsp::MyClass*> ls;createVar__RtCxx(_vdLs, "std::list<my_nsp::MyClass *>", 1)  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:22,33 */ ;

  std::for_each(ls.begin(), ls.end(), [&_vdLs /* lamba表达式的capture, func:, Line:24,Column:39 */](my_nsp::MyClass* k){_VarDeclLs * _vdLs=_init_varLs_inFn__RtCxx("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "", 24, 61); /* 初始化函数变量列表, */ my_nsp::MyClass* idx=k; destroyVarLs_inFn__RtCxx(_vdLs); /* 销毁函数变量列表: */return k; });

  //lambda函数lambdaFunc01 是在当前函数main中定义的 但在另一个函数funcEatLambdaFunc01中执行 因此Lambda函数不能使用使用定义他的函数中的_vdLs、尽量避免使用执行他的函数中的_vdLs以保持简单 即Lambda函数中只用自己内的_vdLs不用任何外部的_vdLs
  my_nsp::MyClass *(*lambdaFunc01)(my_nsp::UserEntity *) = [&_vdLs /* lamba表达式的capture, func:, Line:27,Column:60 */](my_nsp::UserEntity *uEk) {_VarDeclLs * _vdLs=_init_varLs_inFn__RtCxx("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "", 27, 88); /* 初始化函数变量列表, */  my_nsp::UserEntity *uEi = uEk;   destroyVarLs_inFn__RtCxx(_vdLs); /* 销毁函数变量列表: */return (my_nsp::MyClass *)uEk; };
  //Lambda函数 增加的[&_vdLs] 使得情况变得很复杂

  funcEatLambdaFunc01(lambdaFunc01);

  destroyVarLs_inFn__RtCxx(_vdLs); /* 销毁函数变量列表: */return 0;
}


