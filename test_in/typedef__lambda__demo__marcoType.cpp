#define Macro_T_Char char

class Clz_User{

};

union _Union_Score {
int scoreInt;
float scoreFloat;
};

#define Macro_T_Clz_User Clz_User
#define Macro2_T_Clz_User Macro_T_Clz_User
typedef Macro2_T_Clz_User TypeAlias_deepth1_Macro2_T_Clz_User;
typedef TypeAlias_deepth1_Macro2_T_Clz_User TypeAlias_deepth2_Macro2_T_Clz_User;

#define Macro_T_Union_Score _Union_Score
typedef Macro_T_Union_Score TypeAlias_deepth1_Macro_T_Union_Score;
typedef TypeAlias_deepth1_Macro_T_Union_Score TypeAlias_deepth2_Macro_T_Union_Score;
typedef TypeAlias_deepth2_Macro_T_Union_Score TypeAlias_deepth3_Macro_T_Union_Score;
typedef TypeAlias_deepth3_Macro_T_Union_Score TypeAlias_deepth4_Macro_T_Union_Score;
typedef TypeAlias_deepth4_Macro_T_Union_Score TypeAlias_deepth5_Macro_T_Union_Score;

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
  TypeAlias_deepth1_User  zhangsan_user,LISI_user,wangwu_user;
  return;
}

void func_demo_lambda(){
  return;
}
