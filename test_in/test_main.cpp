#include <stdio.h>
int f_int(char* title, int x){
  printf("%s[%d], ",title, x);
  return x;
}

class User{
public:
  int field1;
  int field2;
  int field3;
public:
  User(int x1, int x2, int x3)
  :
    field1(f_int("field1",x1)), field2(f_int("field2",x2)), field3(f_int("field3",x3))
  {
    printf("User_constructor");
  }

};

int main(void){
  User user(10,20,30);
// 输出结果为
//    field1[10], field2[20], field3[30], User_constructor
// 说明  c++构造器初始化列表 执行顺序是 从左边向右边执行的 是 自然顺序, 并不是 从右向左
  return 0;
}