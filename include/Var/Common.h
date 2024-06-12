
#ifndef Var_Common_H
#define Var_Common_H

#define RetTrue_to_KeepOuterLoop return true

//NotModifiable == 不应该被被修改的函数
#define RetFalse_For_NotModifiableFunctionDecl return false
//Modifiable == 能够被修改的函数 == 具有被修改资格的函数
#define RetTrue_For_ModifiableFunctionDecl return true
#endif //Var_Common_H
