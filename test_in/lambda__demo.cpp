
void func_demo(){
  float k;
  auto lambda_func_var_1=[&k]( ){ return 0.0;};
  double (*lambda_func_var_2)() = []() { return 0.0; };
  int float_auto_var2=9.0;
  return;
}
/**

/fridaAnlzAp/clang-var/cmake-build-debug/bin/VarAlone /fridaAnlzAp/clang-var/test_in/lambda__demo.cpp
输出如下
 
typeName='float',typeClass=11,typeClassName=Builtin,isBuiltinType=true
typeName='class (lambda at /fridaAnlzAp/clang-var/test_in/lambda__demo.cpp:4:26)',typeClass=14,typeClassName=Auto,isBuiltinType=false
typeName='double (*)(void)',typeClass=37,typeClassName=Pointer,isBuiltinType=false
typeName='int',typeClass=11,typeClassName=Builtin,isBuiltinType=true
*/