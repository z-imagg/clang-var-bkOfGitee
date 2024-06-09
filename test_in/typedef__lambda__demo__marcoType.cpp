#define Macro_T_Char char

typedef struct _RealType_User_{
  int id;
  char* name;
  bool sex;
} TypeAlias_deepth1_User;

typedef int TypeAlias_deepth1_int;
typedef Macro_T_Char TypeAlias_deepth1_Macro_T_char;
typedef TypeAlias_deepth1_int TypeAlias_deepth2_int;
typedef TypeAlias_deepth2_int TypeAlias_deepth3_int;

void func_demo_typedef(){
  TypeAlias_deepth1_User  zhangsan;
  TypeAlias_deepth3_int age;
  return;
}

void func_demo_lambda(){
  TypeAlias_deepth1_Macro_T_char ch_var;
  float k;
  auto lambda_func_var_1=[&k]( ){ return 0.0;};
  double (*lambda_func_var_2)() = []() { return 0.0; };
  int float_auto_var2=9.0;
  return;
}