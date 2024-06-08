#include "MyClz.h"
#include <algorithm>
#include <list>

void funcEatLambdaFunc01(my_nsp::MyClass *(*func01)(my_nsp::UserEntity *)){
  my_nsp::ScoreRelation sR1;
  func01(nullptr);
  return;
}

int main(int argc, char** argv){
  my_nsp::MyClass varMyClass;
  static Point pnt1;
  struct Point pnt2;
  {
    struct Point * ptr1=&pnt1;
    struct Point * ptr2=&pnt2;
    struct Point pnt3;
  }


  std::list<my_nsp::MyClass*> ls;

  std::for_each(ls.begin(), ls.end(), [](my_nsp::MyClass* k){ my_nsp::MyClass* idx=k; return k; });

  //lambda函数lambdaFunc01 是在当前函数main中定义的 但在另一个函数funcEatLambdaFunc01中执行 因此Lambda函数不能使用使用定义他的函数中的_vdLs、尽量避免使用执行他的函数中的_vdLs以保持简单 即Lambda函数中只用自己内的_vdLs不用任何外部的_vdLs
  my_nsp::MyClass *(*lambdaFunc01)(my_nsp::UserEntity *) = [](my_nsp::UserEntity *uEk) {  my_nsp::UserEntity *uEi = uEk;   return (my_nsp::MyClass *)uEk; };
  funcEatLambdaFunc01(lambdaFunc01);

  return 0;
}



