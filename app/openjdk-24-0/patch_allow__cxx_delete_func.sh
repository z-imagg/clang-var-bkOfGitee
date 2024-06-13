#!/bin/bash

#删除文件 CompileJvm.gmk 中的 '_ZdlPv', 以 允许 jdk源码中出现 c++的 delete语句

#patch文件生成过程:
#0. cp CompileJvm.gmk CompileJvm.gmk.origin
#1. 修改 CompileJvm.gmk
#2. diff -U 3  /d2/jdk-jdk-24-0/make/hotspot/lib/CompileJvm.gmk.origin /d2/jdk-jdk-24-0/make/hotspot/lib/CompileJvm.gmk > /fridaAnlzAp/clang-var/app/openjdk-24-0/CompileJvm.gmk.patch

#3. 应用patch:
patch   /d2/jdk-jdk-24-0/make/hotspot/lib/CompileJvm.gmk  /fridaAnlzAp/clang-var/app/openjdk-24-0/CompileJvm.gmk.patch

#备注
#  _ZdlPv == delete

# jdk编译过程中报错:
#    abstractCompiler.o: Error: Use of global operators new and delete is not allowed in Hotspot:
#                 U operator delete(void*)

# 检测发生在 /d2/jdk-jdk-24-0/build_home/hotspot/variant-server/libjvm/objs/abstractCompiler.o.op_check.cmdline ,  被 CompileJvm.gmk 调用
#    /usr/bin/nm /d2/jdk-jdk-24-0---70507cf4e70e6cb5e8a487ca438f453ae821705c/build_home/hotspot/variant-server/libjvm/objs/abstractCompiler.o 2>&1 | /usr/bin/grep -e _ZdaPv -e _ZdlPv -e _Znam -e _Znwm | /usr/bin/grep ' U '
#      输出为 'U _ZdlPv' 表示 'void delete(void*)'
#     详细如下
#/usr/bin/nm /d2/jdk-jdk-24-0/build_home/hotspot/variant-server/libjvm/objs/abstractCompiler.o
#   输出如下
#                 U CompileThread_lock
#0000000000000000 r .L.str
#0000000000000041 r .L.str.4
#0000000000000067 r .L.str.5
#000000000000007b r .L.str.6
#00000000000000a5 r .L.str.7
#00000000000000b7 r .L.str.8
#                 U _Z16createVar__RtCxxP11__VarDeclLsNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEibi
#                 U _Z23_init_varLs_inFn__RtCxxNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEES4_ii
#                 U _Z24destroyVarLs_inFn__RtCxxP11__VarDeclLs
#                 U _ZdlPv  == 'void delete(void*)'  , 此函数 应该是 /fridaAnlzAp/clang-var/runtime_cpp__vars_fn/runtime_cpp__vars_fn.cpp 中的 delete 语句造成的
#0000000000000000 T _ZN16AbstractCompiler19should_perform_initEv
#0000000000000220 T _ZN16AbstractCompiler23should_perform_shutdownEv
#00000000000003f0 T _ZN16AbstractCompiler9set_stateEi
#                 U _ZN5Mutex4lockEv
#                 U _ZN5Mutex6unlockEv
#                 U _ZN7Monitor10notify_allEv
#                 U _ZN7Monitor4waitEm
#                 U _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERmm
