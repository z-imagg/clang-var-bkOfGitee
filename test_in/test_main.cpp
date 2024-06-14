#pragma message("VarBE_inserted")

class User{};
class Main{
public:
  double func01();
};
int func02(double (*func_ptr) (char,int )){
  char sex;
  int age;
  func_ptr(sex, age);
}
double Main::func01(){

#ifdef  XXX
  User user;
  User & u = user;
#endif
  if(nullptr== nullptr){
    func02([](char ch,int int_){
      User self;createVar__RtCxx(_vdLs/*_VarDeclLs*/,  "class User"/*varTypeName*/, sizeof(self)/*varSize*/ , false/*varIsArr*/ , -1/*arrEleSize*/ )  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:20,16 */ ;  
      return 1.0;
    });
  }
  return 0.0;
}