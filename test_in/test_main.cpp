#pragma message("VarBE_inserted")

int main(int argc, char** argv){_VarDeclLs * _vdLs=_init_varLs_inFn__RtCxx("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "main", 2, 32); /* 初始化函数变量列表, */
  extern void func02();// 这里没有createVar__RtCxx ,即为 正常, 已跳过函数体内的函数签名语句
  destroyVarLs_inFn__RtCxx(_vdLs); /* 销毁函数变量列表: */return 0;
}



