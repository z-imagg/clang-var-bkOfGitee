
void func_demo(){
  float k;
  auto lambda_func_var_1=[&k]( ){ return 0.0;};
  double (*lambda_func_var_2)() = []() { return 0.0; };
  auto float_auto_var2=9.0;
  return;
}