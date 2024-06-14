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
double Main::func01(){_VarDeclLs * _vdLs=_init_varLs_inFn__RtCxx("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "Main::func01", 12, 22); /* 初始化函数变量列表, */

#ifdef  XXX
  User user;
  User & u = user;
#endif
      User self;createVar__RtCxx(_vdLs/*_VarDeclLs*/,  "class User"/*varTypeName*/, sizeof(self)/*varSize*/ , false/*varIsArr*/ , -1/*arrEleSize*/ )  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:18,16 */ ;  
  destroyVarLs_inFn__RtCxx(_vdLs); /* 销毁函数变量列表: */return 0.0;
}