#pragma message("VarBE_inserted")
 #include "runtime_cpp__vars_fn.h"
class T_User {};
int main(int argc, char** argv){
 _VarDeclLs * _vdLs=_init_varLs_inFn__RtCxx("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "main", 2, 32); /* 初始化函数变量列表, */
  T_User zhangsan;
 createVar__RtCxx(_vdLs/*_VarDeclLs*/,  "class T_User"/*varTypeName*/, sizeof(zhangsan)/*varSize*/ , false/*varIsArr*/ , -1/*arrEleSize*/ )  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:3,18 */ ;  
  int s;
  while(T_User * ptr=&zhangsan){//while循环变量 后 不会再被插入createVar语句了
      s++;
  }
  macroNoOutBuf__destroyVarLs_inFn__RtCxx(_vdLs); /* 销毁函数变量列表: */ 
return 0;
}