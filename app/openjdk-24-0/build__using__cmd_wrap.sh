

### cmd-wrap带clang-var插件 编译

(cd /app/cmd-wrap/cfg; cp my_config.clPlgVar.py my_config.py)


source /app/cmd-wrap/script/cmd_setup.sh

cd /d2/jdk-jdk-24-0/build_home/
rm -fr *

chmod +x /d2/jdk-jdk-24-0/configure

CC=/usr/bin/clang CXX=/usr/bin/clang++ ../configure  --prefix=/d2/jdk-jdk-24-0.install-home --with-boot-jdk=/app/zulu23.0.53-beta-jdk23.0.0-beta.21-linux_x64/     --with-toolchain-type=clang

# make hotspot JOBS=6
make jdk JOBS=1


#/d2/jdk-jdk-24-0/build_home/
./jdk/bin/javac
echo $? # 退出代码为2, 是正常的
# 经过验证 /app/jdk8/bin/javac 的退出代码也是2, 所以上一行是正常的
