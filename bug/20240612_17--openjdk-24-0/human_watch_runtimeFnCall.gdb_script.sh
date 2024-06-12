# gdb脚本编写参考 https://gitcode.net/myz/malloc_hok/glibc_mallocHok_gdbScript/-/tree/z/glibc-2.27-MA%E6%97%A5%E5%BF%97-exit%E5%B0%BE%E5%86%99%E6%97%A5%E5%BF%97/R/__run_exit_handlers-atexit-end

# gdb -q --command=/fridaAnlzAp/clang-var/bug/20240612_17--openjdk-24-0/human_watch_runtimeFnCall.gdb_script.sh  --args /d2/jdk-jdk-24-0/build_home/hotspot/variant-server/tools/adlc/adlc -q -T -DLINUX=1 -D_GNU_SOURCE=1 -g -DAMD64=1 -D_LP64=1 -DNDEBUG -DPRODUCT /d2/jdk-jdk-24-0/build_home/hotspot/variant-server/support/adlc/all-ad-src.ad -c/d2/jdk-jdk-24-0/build_home/hotspot/variant-server/support/adlc/ad_x86.cpp -h/d2/jdk-jdk-24-0/build_home/hotspot/variant-server/support/adlc/ad_x86.hpp -a/d2/jdk-jdk-24-0/build_home/hotspot/variant-server/support/adlc/dfa_x86.cpp -v/d2/jdk-jdk-24-0/build_home/hotspot/variant-server/support/adlc/adGlobals_x86.hpp

set print pretty on
set pagination off
set breakpoint pending on


set $counter=0

break _init_varLs_inFn__RtCxx
commands
 silent
 set $counter=$counter+1
 if $counter % 100 == 0
 printf "_init_varLs_inFn__RtCxx \n"
 end
#  bt
 continue
end


run
contine
quit