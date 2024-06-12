
#include <clang/AST/ASTTypeTraits.h>
#include <clang/AST/Decl.h>
#include "Var/ClConst.h"

const  clang::ASTNodeKind  ClConstAstNodeKind::functionDecl = clang::ASTNodeKind::getFromNodeKind<clang::FunctionDecl>();
