typedef struct _RealType_User_{
  int id;
  char* name;
  bool sex;
} TypeAlias_deepth1_User;

typedef int TypeAlias_deepth1_int;
typedef TypeAlias_deepth1_int TypeAlias_deepth2_int;
typedef TypeAlias_deepth2_int TypeAlias_deepth3_int;

void func01(){
  TypeAlias_deepth1_User  zhangsan;
  TypeAlias_deepth3_int age;
  return;
}