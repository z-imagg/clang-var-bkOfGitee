#include "clang/Lex/Lexer.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>



#include <clang/Frontend/FrontendActions.h>

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "Brc/BrcAstCnsm.h"

using namespace llvm;
using namespace clang;
using namespace clang;


int main() {
  // 假设您已经有一个源码字符串，名为Code
  std::string Code = "int x = 5;";

  // 创建一个SourceManager对象
  SourceManager SM;

  // 将源码字符串添加到SourceManager中
  FileID File = SM.createFileIDForMemBuffer(
          llvm::MemoryBuffer::getMemBuffer(Code), clang::SrcMgr::C_User);

  // 获取源码字符串的起始位置
  SourceLocation StartLoc = SM.getLocForStartOfFile(File);

  // 获取源码字符串的结束位置
  SourceLocation EndLoc = SM.getLocForEndOfFile(File);

  // 获取源码字符串的token的结束位置
  SourceLocation TokenEndLoc = Lexer::getLocForEndOfToken(EndLoc, 0, SM, LangOptions());

  // 输出结果
  std::cout << "Code: " << Code << std::endl;
  std::cout << "Token end location: ";
  TokenEndLoc.print(std::cout, SM);
  std::cout << std::endl;

  return 0;
}
