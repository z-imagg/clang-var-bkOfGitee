# gdb脚本编写参考 https://gitcode.net/myz/malloc_hok/glibc_mallocHok_gdbScript/-/tree/z/glibc-2.27-MA%E6%97%A5%E5%BF%97-exit%E5%B0%BE%E5%86%99%E6%97%A5%E5%BF%97/R/__run_exit_handlers-atexit-end



#操作步骤
#0. 新开终端Z执行htop 观看cpu 和 内存占用

#1. 新开终端A执行以下命令
#   一开始会因为无进程adlc而报错 可以忽略
# 间隔20毫秒发送一个信号SIGUSR1给进程adlc
# while true; do kill -SIGUSR1 $(pidof adlc); sleep 0.020; done

#2. 新开终端B, 以gdb 调试adlc 并调用此脚本
# gdb -q --command=/fridaAnlzAp/clang-var/bug/20240612_17--openjdk-24-0/human_watch_runtimeFnCall.gdb_script.sh  --args /d2/jdk-jdk-24-0/build_home/hotspot/variant-server/tools/adlc/adlc -q -T -DLINUX=1 -D_GNU_SOURCE=1 -g -DAMD64=1 -D_LP64=1 -DNDEBUG -DPRODUCT /d2/jdk-jdk-24-0/build_home/hotspot/variant-server/support/adlc/all-ad-src.ad -c/d2/jdk-jdk-24-0/build_home/hotspot/variant-server/support/adlc/ad_x86.cpp -h/d2/jdk-jdk-24-0/build_home/hotspot/variant-server/support/adlc/ad_x86.hpp -a/d2/jdk-jdk-24-0/build_home/hotspot/variant-server/support/adlc/dfa_x86.cpp -v/d2/jdk-jdk-24-0/build_home/hotspot/variant-server/support/adlc/adGlobals_x86.hpp

#3. 终端B被gdb占有,  输入 run , 步骤1 将实际开始间隔20毫秒发送 信号SIGUSR1给gdb此脚本
#   从而 间接地 使得 此脚本 间隔20毫秒执行一次 断点处理过程

#执行一次以上步骤 , 获知 当counter==350的时候 , 内存吃了大约4GB, 此时开始打印调用栈
#   看调用栈并没有死循环, 因此 排除死循环
#     剩下另一个排查方案:
#       将 局限范围在adlc依赖的源码文件们中 , 比较 上一次正常的clang-var修改代码 和 此次clang-var修改代码(崩溃)   人工观看二者差异,
#       调整差异量  从新编译 adlc 观看是否崩溃 ,
#       在去掉某个函数的修改后 adlc 恰好不崩溃, 则 就是该函数的修改有问题
set print pretty on
set pagination off
set breakpoint pending on


set $counter=0

#  "stop", "nostop", "print", "noprint","pass", "nopass", "ignore", or "noignore".

handle SIGUSR1 nostop nopass
catch signal SIGUSR1
commands 1
 silent
 set $counter=$counter+1
 printf "counter= %d \n", $counter
 if $counter >= 350
 backtrace
 end
 continue
end



# run #手工输入run
# continue
# quit