LLVM15_HOME=/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4
export PATH=$LLVM15_HOME/bin:$PATH

_Home_=/fridaAnlzAp/clang-var/test_in
_SyntaxTree_dir_=$_Home_/ast-dump.syntax-only/
mkdir -p $_SyntaxTree_dir_

#显示源文件test_main.cpp的语法树
#  彩色输出，方便人工查看
#clang -Xclang -ast-dump -fsyntax-only   $_Home_/test_main.cpp
#  文本输出，方便保存
#clang -Xclang -ast-dump -fsyntax-only  -fno-color-diagnostics $_Home_/test_main.cpp   | tee /tmp/test_main.cpp.ast-dump.syntax-only.txt

cd $_Home_ ; find . -type f -name "*.cpp" | xargs -I@ bash -c "clang -Xclang -ast-dump -fsyntax-only  -fno-color-diagnostics  @ > $_SyntaxTree_dir_/@.syntax_tree.txt"


