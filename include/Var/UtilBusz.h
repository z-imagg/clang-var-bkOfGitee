

#ifndef UtilFuncDeclIsModifiable_H
#define UtilFuncDeclIsModifiable_H

#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
using namespace clang;

class UtilBusz {
public:
  static bool isModifiable_FunctionDecl(CXXMethodDecl* cxxMethDecl,  SourceManager& SM );
};


#endif //UtilFuncDeclIsModifiable_H
