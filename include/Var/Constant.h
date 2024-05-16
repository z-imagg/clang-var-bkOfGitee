
#ifndef Var_CONSTANT_H
#define Var_CONSTANT_H
#include <string>


class Constant{
public:
    const   std::string PrgMsgStmt_funcIdAsmIns = "#pragma message(\"VarBE_inserted\")\n";

    //CollectIncMacro_PPCb::pragmaMsgFull决定了Constant::NameSpace_funcIdAsmIns的值的样式是 "命名空间:pragmaMessgae"
    const   std::string NameSpace_funcIdAsmIns = ":VarBE_inserted";

};
#endif //Var_CONSTANT_H