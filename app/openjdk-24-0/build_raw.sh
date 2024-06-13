## openjdk-24编译
#https://github.com/openjdk/jdk/archive/refs/tags/jdk-24+0.zip


#https://www.azul.com/downloads/#downloads-table-zulu , 下载jdk-23作为boot-jdk
#wget https://cdn.azul.com/zulu/bin/zulu23.0.53-beta-jdk23.0.0-beta.21-linux_x64.tar.gz
# JDK23_HOME == /app/zulu23.0.53-beta-jdk23.0.0-beta.21-linux_x64/

#依赖
sudo apt-get install libasound2-dev libcups2-dev libfontconfig1-dev
sudo apt-get install libx11-dev libxext-dev libxrender-dev libxrandr-dev libxtst-dev libxt-dev


#编译步骤
( cd /d2;  wget https://github.com/openjdk/jdk/archive/refs/tags/jdk-24+0.zip ;)

mkdir /d2/jdk-jdk-24-0/build_home/
cd /d2/jdk-jdk-24-0/build_home/

chmod +x /d2/jdk-jdk-24-0/configure

_ClangHome=/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4
CC=$_ClangHome/bin/clang CXX=$_ClangHome/bin/clang++ ../configure  --prefix=/d2/jdk-jdk-24-0.install-home --with-boot-jdk=/app/zulu23.0.53-beta-jdk23.0.0-beta.21-linux_x64/     --with-toolchain-type=clang

# make hotspot JOBS=6
make jdk JOBS=6



#编译产物运行
cd /d2/jdk-jdk-24-0/build_home/
./jdk/bin/javac
