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

  User myself;createVar__RtCxx(_vdLs/*_VarDeclLs*/,  "class User"/*varTypeName*/, sizeof(myself)/*varSize*/ , false/*varIsArr*/ , -1/*arrEleSize*/ )  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:14,14 */ ;  
#ifdef  XXX
  User user;
  User & u = user;
#endif
    func02([](char ch,int int_){_VarDeclLs * _vdLs=_init_varLs_inFn__RtCxx("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "", 19, 32); /* 初始化函数变量列表, */
      User self;createVar__RtCxx(_vdLs/*_VarDeclLs*/,  "class User"/*varTypeName*/, sizeof(self)/*varSize*/ , false/*varIsArr*/ , -1/*arrEleSize*/ )  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:20,16 */ ;  
      destroyVarLs_inFn__RtCxx(_vdLs); /* 销毁函数变量列表: */return 1.0;
    });
  destroyVarLs_inFn__RtCxx(_vdLs); /* 销毁函数变量列表: */return 0.0;
}
