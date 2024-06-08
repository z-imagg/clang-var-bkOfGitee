# 核心问题:  sleuthkit-4.12.1 中的 libtsk.la 要求 对 libclangPlgVar_runtime_c_TmPnt_ThrLcl.a 加 选项 '-fPIC' 重新编译 lib*c_TmPnt_ThrLcl.a 
# 核心报错:  /usr/bin/ld: /fridaAnlzAp/clang-var/runtime_c__TmPnt_ThreadLocal/build/libclangPlgVar_runtime_c_TmPnt_ThrLcl.a(rntm_c__TmPnt_ThrLcl.c.o): relocation R_X86_64_TPOFF32 against symbol `TL_TmPnt' can not be used when making a shared object; recompile with -fPIC


# make[3]: 进入目录“/fridaAnlzAp/sleuthkit-4.12.1/tsk”

cd /fridaAnlzAp/sleuthkit-4.12.1/tsk/

/bin/bash ../libtool  --tag=CC   --mode=link /usr/bin/clang -Wall -Wextra -Wno-unused-parameter -g -O2 -version-info 21:1:2  -L/usr/local/lib -o libtsk.la -rpath /usr/local/lib  base/libtskbase.la img/libtskimg.la vs/libtskvs.la fs/libtskfs.la hashdb/libtskhashdb.la auto/libtskauto.la pool/libtskpool.la util/libtskutil.la -ldl -lstdc++ 


# libtool: link: /usr/bin/clang -shared  -fPIC -DPIC  -Wl,--whole-archive base/.libs/libtskbase.a img/.libs/libtskimg.a vs/.libs/libtskvs.a fs/.libs/libtskfs.a hashdb/.libs/libtskhashdb.a auto/.libs/libtskauto.a pool/.libs/libtskpool.a util/.libs/libtskutil.a -Wl,--no-whole-archive  -L/usr/local/lib -ldl -lstdc++  -g -O2   -Wl,-soname -Wl,libtsk.so.19 -o .libs/libtsk.so.19.2.1
# /usr/bin/ld: /fridaAnlzAp/clang-var/runtime_c__TmPnt_ThreadLocal/build/libclangPlgVar_runtime_c_TmPnt_ThrLcl.a(rntm_c__TmPnt_ThrLcl.c.o): relocation R_X86_64_TPOFF32 against symbol `TL_TmPnt' can not be used when making a shared object; recompile with -fPIC
# /usr/bin/ld: failed to set dynamic section sizes: bad value
# clang-15: error: linker command failed with exit code 1 (use -v to see invocation)



ls -lh /tmp/*.log*
# -rw-rw-r-- 1 z z 3.5K  6月  8 23:03 /tmp/clang-1717858985539321856-20240608230305.539322-169557.log
# lrwxrwxrwx 1 z z   63  6月  8 23:03 /tmp/clang-1717858985539321856-20240608230305.539322-169557.log--errorCode_1 -> /tmp/clang-1717858985539321856-20240608230305.539322-169557.log

cat /tmp/clang-1717858985539321856-20240608230305.539322-169557.log
# INFO:2024-06-08 23:03:05.539407@/app/cmd-wrap/py_util/global_var.py:73:__init__:生成唯一文件名成功/tmp/clang-1717858985539321856-20240608230305.539322-169557.log,作为日志文件. initCurDir=【/fridaAnlzAp/sleuthkit-4.12.1/tsk】
# INFO:2024-06-08 23:03:05.539473@/usr/bin/clang:68:<module>:收到命令及参数:【/usr/bin/clang -shared -fPIC -DPIC -Wl,--whole-archive base/.libs/libtskbase.a img/.libs/libtskimg.a vs/.libs/libtskvs.a fs/.libs/libtskfs.a hashdb/.libs/libtskhashdb.a auto/.libs/libtskauto.a pool/.libs/libtskpool.a util/.libs/libtskutil.a -Wl,--no-whole-archive -L/usr/local/lib -ldl -lstdc++ -g -O2 -Wl,-soname -Wl,libtsk.so.19 -o .libs/libtsk.so.19.2.1】, 父进程完成命令行【/bin/bash../libtool--tag=CC--mode=link/usr/bin/clang-Wall-Wextra-Wno-unused-parameter-g-O2-version-info21:1:2-L/usr/local/lib-olibtsk.la-rpath/usr/local/libbase/libtskbase.laimg/libtskimg.lavs/libtskvs.lafs/libtskfs.lahashdb/libtskhashdb.laauto/libtskauto.lapool/libtskpool.lautil/libtskutil.la-ldl-lstdc++】
# INFO:2024-06-08 23:03:05.539522@/app/cmd-wrap/CxxCmdParser.py:68:cxxCmdParse:'简易'即将解析文本singleGccCmd：【/usr/bin/clang -shared -fPIC -DPIC -Wl,--whole-archive base/.libs/libtskbase.a img/.libs/libtskimg.a vs/.libs/libtskvs.a fs/.libs/libtskfs.a hashdb/.libs/libtskhashdb.a auto/.libs/libtskauto.a pool/.libs/libtskpool.a util/.libs/libtskutil.a -Wl,--no-whole-archive -L/usr/local/lib -ldl -lstdc++ -g -O2 -Wl,-soname -Wl,libtsk.so.19 -o .libs/libtsk.so.19.2.1】
# INFO:2024-06-08 23:03:05.539530@/app/cmd-wrap/CxxCmdParser.py:69:cxxCmdParse:命令中的源文件相关字段为: srcFpIsDevNull False ,  has_m16 False , input_is_std_in None  
# INFO:2024-06-08 23:03:05.539543@/usr/bin/clang:102:<module>:因为此命令中无源文件名，故而不拦截此命令
# INFO:2024-06-08 23:03:05.539553@/app/cmd-wrap/interceptor_util.py:60:execute_BCmd:构造出目命令:【/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang -shared -fPIC -DPIC -Wl,--whole-archive base/.libs/libtskbase.a img/.libs/libtskimg.a vs/.libs/libtskvs.a fs/.libs/libtskfs.a hashdb/.libs/libtskhashdb.a auto/.libs/libtskauto.a pool/.libs/libtskpool.a util/.libs/libtskutil.a -Wl,--no-whole-archive -L/usr/local/lib -ldl -lstdc++ -g -O2 -Wl,-soname -Wl,libtsk.so.19 -o .libs/libtsk.so.19.2.1 /fridaAnlzAp/clang-var/runtime_c__TmPnt_ThreadLocal/build/libclangPlgVar_runtime_c_TmPnt_ThrLcl.a /fridaAnlzAp/clang-var/runtime_c__vars_fn/build/libclangPlgVar_runtime_c.a /app/clibs--list/build/libclibs_list.a /fridaAnlzAp/clang-var/runtime_c__TmPnt_ThreadLocal/build/libclangPlgVar_runtime_c_TmPnt_ThrLcl.a /fridaAnlzAp/clang-var/build/runtime_cpp__vars_fn/libclangPlgVar_runtime_cxx.a】
# INFO:2024-06-08 23:03:05.642155@/app/cmd-wrap/interceptor_util.py:92:execute_BCmd:真实命令退出码,异常退出码:【1】
# INFO:2024-06-08 23:03:05.642188@/app/cmd-wrap/interceptor_util.py:97:execute_BCmd:真实命令标准输出【】
# INFO:2024-06-08 23:03:05.642212@/app/cmd-wrap/interceptor_util.py:100:execute_BCmd:真实命令错误输出【/usr/bin/ld: /fridaAnlzAp/clang-var/runtime_c__TmPnt_ThreadLocal/build/libclangPlgVar_runtime_c_TmPnt_ThrLcl.a(rntm_c__TmPnt_ThrLcl.c.o): relocation R_X86_64_TPOFF32 against symbol `TL_TmPnt' can not be used when making a shared object; recompile with -fPIC
# /usr/bin/ld: failed to set dynamic section sizes: bad value
# clang-15: error: linker command failed with exit code 1 (use -v to see invocation)
# 】
