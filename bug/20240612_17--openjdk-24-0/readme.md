## cmd-wrap带clang-var插件 编译

```shell
(cd /app/cmd-wrap/cfg; cp my_config.clPlgVar.py my_config.py)


source /app/cmd-wrap/script/cmd_setup.sh

cd /d2/jdk-jdk-24-0/build_home/
rm -fr *

CC=/usr/bin/clang CXX=/usr/bin/clang++ ../configure  --prefix=/d2/jdk-jdk-24-0.install-home --with-boot-jdk=/app/zulu23.0.53-beta-jdk23.0.0-beta.21-linux_x64/     --with-toolchain-type=clang

make jdk  JOBS=1
#发生以下错误
```



## 问题

### openjdk-24-0的adlc占用100%cpu和100%内存

```htop```

命令如下 执行时 被操作系统killed
```shell
 /d2/jdk-jdk-24-0/build_home/hotspot/variant-server/tools/adlc/adlc -q -T -DLINUX=1 -D_GNU_SOURCE=1 -g -DAMD64=1 -D_LP64=1 -DNDEBUG -DPRODUCT /d2/jdk-jdk-24-0/build_home/hotspot/variant-server/support/adlc/all-ad-src.ad -c/d2/jdk-jdk-24-0/build_home/hotspot/variant-server/support/adlc/ad_x86.cpp -h/d2/jdk-jdk-24-0/build_home/hotspot/variant-server/support/adlc/ad_x86.hpp -a/d2/jdk-jdk-24-0/build_home/hotspot/variant-server/support/adlc/dfa_x86.cpp -v/d2/jdk-jdk-24-0/build_home/hotspot/variant-server/support/adlc/adGlobals_x86.hpp
```

```htop``` 并重新执行该命令 , 发现其单cpu核心100%占用,  全部8G内存被吃尽, 因为 怀疑clang-var插件在不合理的地方插入了runtime_c*调用 ,导致死循环或死递归

因此 问题 可以归类为 '不提前退出上层循环后引发的副作用'