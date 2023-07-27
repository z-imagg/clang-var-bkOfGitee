
import pathlib
from pathlib import Path
import re
from typing import List, Tuple


class File:
    def __init__(self,file_path:str):
        self.file_path:str=file_path
        self.栈生:int=0
        self.栈死:int=0
        self.堆生:int=0
        self.堆死:int=0
    def 栈分配(self, 变量个数):
        self.栈生+=变量个数
    def 栈释放(self, 变量个数):
        self.栈死+=变量个数
    def 堆分配(self, 变量个数):
        self.堆生+=变量个数
    def 堆释放(self, 变量个数):
        self.堆死+=变量个数
    def 栈平衡性(self)->bool:
        栈平衡:bool = self.栈生==self.栈死
        return 栈平衡
    def 堆平衡性(self)->bool:
        堆平衡:bool = self.堆生==self.堆死
        return 堆平衡
    def 平衡性(self)->bool:
        栈平衡:bool =self.栈平衡性()
        堆平衡:bool =self.堆平衡性()
        return 栈平衡 and 堆平衡

    def 失衡方向(self)->Tuple[bool,bool,str,str]:
        栈平衡:bool =self.栈平衡性()
        堆平衡:bool =self.堆平衡性()

        栈生小于死吗:bool = self.栈生<self.栈死
        堆生小于死吗:bool = self.堆生<self.堆死
        栈失衡方向描述:str="" if 栈平衡 else  ("栈死多" if 栈生小于死吗 else "栈生多")
        堆失衡方向描述:str="" if 堆平衡 else  ( "堆死多" if 堆生小于死吗 else "堆生多")
        return (栈生小于死吗,堆生小于死吗,栈失衡方向描述,堆失衡方向描述)

    def __str__(self):
        _,_,栈失衡方向描述,堆失衡方向描述=self.失衡方向()
        return f"{self.file_path},栈[{栈失衡方向描述},生:{self.栈生},死:{self.栈死}];堆:[{堆失衡方向描述},生:{self.堆生},死:{self.堆死}];"
import glob
import regex as re
#  `regex`库：`regex`库是一个功能更强大且兼容性更好的正则表达式库，它提供了对PCRE（Perl兼容正则表达式）的支持。与标准库中的`re`模块相比，`regex`库提供了更多的功能和选项，例如更复杂的模式匹配、更多的元字符和修饰符等。您可以使用`pip install regex`命令安装该库。
from pathlib import Path

pattern = r'X__t_clock_tick\(/[*]栈生[*]/(\d+), /[*]栈死[*]/(\d+), /[*]堆生[*]/(\d+), /[*]堆死[*]/(\d+)\);'
def lineLsProcess(lines:List[str],fK:File):
    # 遍历每行
    for line in lines:
        # 匹配行中的 X__t_clock_tick(/*栈生*/数A, /*栈死*/数B, /*堆生*/数C, /*堆死*/数D);
        match = re.search(pattern, line)
        if match:
            # 提取数A、数B、数C、数D
            fK.栈分配(int(match.group(1)))
            fK.栈释放(int(match.group(2)))
            fK.堆分配(int(match.group(3)))
            fK.堆释放(int(match.group(4)))
            # print(f"数A: {栈生}, 数B: {栈死}, 数C: {堆生}, 数D: {堆死}")
    return

def process_files(directory):
    # 递归遍历目录下的所有.c和.cpp文件
    file_pattern = f"{directory}/**/*.c"  # 匹配.c文件
    files = glob.glob(file_pattern, recursive=True)

    file_pattern = f"{directory}/**/*.cpp"  # 匹配.cpp文件
    files += glob.glob(file_pattern, recursive=True)

    # 遍历每个源文件
    for file_path in files:
        fK:File=File(file_path)
        with open(file_path, 'r') as f:
            lines = f.readlines()
        lineLsProcess(lines,fK)
        if not fK.平衡性():
            print(f"失衡:{fK}")
        else:
            # print(f"平衡:{fK}")
            pass
###############



# 使用示例
if __name__ == '__main__':
    process_files("/pubx/llvm-project/llvm/")


"""20230727 22:55 llvm添加的滴答语句 平衡检查结果:
失衡:/pubx/llvm-project/llvm/lib/Support/BLAKE3/blake3.c,栈[栈死多,生:53,死:56];堆:[,生:0,死:0];
失衡:/pubx/llvm-project/llvm/tools/llvm-rc/ResourceScriptParser.cpp,栈[栈死多,生:49,死:113];堆:[,生:0,死:0];
失衡:/pubx/llvm-project/llvm/lib/CodeGen/RegisterCoalescer.cpp,栈[栈生多,生:362,死:361];堆:[,生:0,死:0];
失衡:/pubx/llvm-project/llvm/lib/DebugInfo/Symbolize/MarkupFilter.cpp,栈[栈死多,生:19,死:33];堆:[,生:0,死:0];
失衡:/pubx/llvm-project/llvm/lib/ProfileData/InstrProfReader.cpp,栈[栈死多,生:106,死:109];堆:[,生:0,死:0];
失衡:/pubx/llvm-project/llvm/lib/IR/DebugInfoMetadata.cpp,栈[栈生多,生:95,死:70];堆:[,生:0,死:0];
失衡:/pubx/llvm-project/llvm/lib/Object/WindowsResource.cpp,栈[栈死多,生:81,死:93];堆:[,生:0,死:0];
失衡:/pubx/llvm-project/llvm/lib/Bitcode/Writer/BitcodeWriter.cpp,栈[栈死多,生:342,死:373];堆:[,生:0,死:0];
失衡:/pubx/llvm-project/llvm/lib/Support/UnicodeNameToCodepoint.cpp,栈[栈死多,生:81,死:82];堆:[,生:0,死:0];
失衡:/pubx/llvm-project/llvm/lib/Support/BuryPointer.cpp,栈[栈死多,生:3,死:4];堆:[,生:0,死:0];
失衡:/pubx/llvm-project/llvm/lib/AsmParser/LLParser.cpp,栈[栈死多,生:795,死:993];堆:[,生:0,死:0];
失衡:/pubx/llvm-project/llvm/lib/Target/X86/X86ISelLowering.cpp,栈[栈死多,生:8274,死:8275];堆:[,生:0,死:0];
失衡:/pubx/llvm-project/llvm/lib/Frontend/OpenMP/OMPIRBuilder.cpp,栈[栈死多,生:811,死:825];堆:[,生:0,死:0];
失衡:/pubx/llvm-project/llvm/lib/Transforms/IPO/AttributorAttributes.cpp,栈[栈死多,生:1016,死:1019];堆:[,生:0,死:0];
失衡:/pubx/llvm-project/llvm/lib/Transforms/IPO/OpenMPOpt.cpp,栈[栈死多,生:569,死:616];堆:[,生:0,死:0];
"""

#这是纸面不平衡，确实要处理。但纸面不平衡 这里的结果是 死多 生少，而现象是 栈生在积累。  问题是出在： return语句 应该释放 本函数当前已经申请的所有栈变量。

# 以下文本 贴给 https://chat.chatgptdemo.net/  ， 可以获得大致代码
# python编程：请编写通用函数（不准使用for，只能使用map()、list()、列表解析等高级特性； 不准使用os.walk， 尽可能glob、pathlib；不准使用自带re，尽可能使用三方库re2 ），递归遍历给定目录下所有 .c、.cpp 源文件， 读取每个源文件，遍历该源文件中每行，  匹配行中的 X__t_clock_tick(/*栈生*/数A, /*栈死*/数B, /*堆生*/数C, /*堆死*/数D);   ， 若有匹配  打印 数A 、数B、 数C、 数D


