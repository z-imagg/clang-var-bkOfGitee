
#include "runtime_c__vars_fn.h"
#include "rntm_c__TmPnt_ThrLcl.h"
#include <malloc.h>
#include <string.h>

// 本模块 runtime_c__vars_fn 只允许被C使用，而不允许被C++使用
#ifdef __cplusplus
// 本模块若被C++使用, 则报错以终止编译
#error "[ERROR] runtime_c__vars_fn must be used as C not C++"
#else
// 本模块必须被C使用


#define _NEW_(Type)  ( (Type*)malloc(sizeof(Type)) )
#define _DEL_(ptr)   free(ptr); ptr=NULL

#include "sds.h"
#include "list.h"

//向外报出错误，方便gdb以此加断点，从而直接到达错误现场
int __debugVar__ErrCode__runtime_c__vars_fn=0;
#define Err01_Beyond_JsonTxtOutLimit 11

//危险隔离带长度
#define __Gap_Danger_Char_Cnt 9

//函数左花括号后 插入 'void* _varLs_ptr=_init_varLs_inFn__RtC00();'
_VarDeclLs *  _init_varLs_inFn__RtC00(sds srcFilePath, sds funcName, int funcLBrc_line, int funcLBrc_column){
    //new:HeapObj_vdLs
    _VarDeclLs *vdLs = _NEW_(_VarDeclLs); // 分配 对象0 : _NEW_0  //new _VarDeclLs;
    //new:HeapObj_vdVec
    vdLs->_vdVec=list_new();//new std::vector<_VarDecl>();

    vdLs->srcFilePath=srcFilePath;
    vdLs->funcName=funcName;
    vdLs->funcLBrc_line=funcLBrc_line;
    vdLs->funcLBrc_column=funcLBrc_column;

    //依赖模块 runtime_c__TmPnt_ThreadLocal
    vdLs->TmPnt=TL_TmPnt__get();

    return vdLs;
}


//结构体变量声明末尾 插入 'createVar__RtC00(_varLs_ptr,"变量类型名",变量个数);'
void createVar__RtC00(_VarDeclLs *_vdLs, sds varTypeName, int varSize,short varIsArr,int arrEleSize){

    _VarDecl *vd= _NEW_(_VarDecl);//分配 对象1 : _NEW_1
    vd->varTypeName=varTypeName;
    vd->varSize=varSize;
    vd->varIsArr=varIsArr;
    vd->arrEleSize=arrEleSize;

    list_node_t *node=list_node_new(vd);
    list_rpush(_vdLs->_vdVec, node); // vdLs->_vdVec->push_back(vd);
}

#define _sds_jsonItem(jsnTxt, fieldName, fieldVal)      sdscat(jsnTxt, "\"");; sdscat(jsnTxt, fieldName);; sdscat(jsnTxt, "\":");; sdscat(jsnTxt, fieldVal);
#define sds_jsonItemEnd(jsnTxt, fieldName, fieldVal)       _sds_jsonItem(jsnTxt, fieldName, fieldVal);  sdscat(jsnTxt, "\"");;
#define sds_jsonItem(jsnTxt, fieldName, fieldVal)          _sds_jsonItem(jsnTxt, fieldName, fieldVal);  sdscat(jsnTxt, "\",");;

//【销毁变量通知】 函数右花括号前 插入 'destroyVarLs_inFn__RtC00(_varLs_ptr);'
void destroyVarLs_inFn__RtC00(_VarDeclLs *_vdLs, int jsonTxtOutLimit, char* jsonTxtOut_, int* jsonTxtOutLen_){
    list_t* _vdVec = _vdLs->_vdVec; // std::vector<_VarDecl>

  long varDeclCnt = _vdVec->len; //std::distance(_vdVec->begin(), _vdVec->end());

//变量jsonTxt 、变量jsonTxt_vdItem 用于 组装_vdLs为json并输出到控制台
if(varDeclCnt>0){
  sds jsonTxt = sdsempty();
  jsonTxt=sdscat(jsonTxt,"{\"vdLs\":[");

  int varSizeSum=0;
  list_node_t *nodeK;
  list_iterator_t *it = list_iterator_new(_vdVec, LIST_HEAD);
  while ((nodeK = list_iterator_next(it))) {
    _VarDecl* vdK=(_VarDecl*)(nodeK->val); //这不是c++，这是c，无类型信息，只能做危险的强制类型转换
    sds jsonTxt_vdItem   =sdscatprintf(sdsempty(), "{\"varTypeName\":\"%s\", \"varSize\":%d, \"varIsArr\":%d, \"arrEleSize\":%d}", vdK->varTypeName, vdK->varSize, vdK->varIsArr, vdK->arrEleSize);
    jsonTxt=sdscatprintf(jsonTxt,"%s", jsonTxt_vdItem);//现在这是sdscatprintf的 正确语义；原本将 目标参数和来源参数都为jsonTxt 是错误语义（导致指数爆炸增长，最终肯定会缓冲区溢出）
    //若sds类型的格式化串用%S ，则这里sdscatprintf返回NULL， 改为%s则结果正常
    varSizeSum += vdK->varSize;
    _DEL_(vdK);//释放 对象1 : _DEL_1
  }
  //释放迭代器(不释放链表自身)
  list_iterator_destroy(it);

  jsonTxt=sdscatprintf(jsonTxt,"\"srcFilePath\":\"%s\", \"funcLBrc_line:\":%d, \"funcLBrc_column\":%d, \"varDeclCnt\":%d, \"varSizeSum\":%d\n", _vdLs->srcFilePath , _vdLs->funcLBrc_line , _vdLs->funcLBrc_column , varDeclCnt, varSizeSum ) ;
  int jsonTxtLen=(int)sdslen(jsonTxt);

  // 将json串返回给调用者
  //如果给出的json文本长度超出 frida提供的缓冲区jsonTxtOut_的尺寸jsonTxtOutLimit ，则直接退出当前进程
  //   解决办法是frida使用更大的缓冲区jsonTxtOut_
  if(jsonTxtLen > jsonTxtOutLimit - __Gap_Danger_Char_Cnt){
    //写调试变量, gdb可以以此加条件断点'break runtime_c__vars_fn.c:96 if __debugVar__ErrCode__runtime_c__vars_fn=11(Err01_Beyond_JsonTxtOutLimit)'
    __debugVar__ErrCode__runtime_c__vars_fn=Err01_Beyond_JsonTxtOutLimit;
    printf("[Err01_Beyond_JsonTxtOutLimit] ,jsonTxtLen=[%d],jsonTxtOutLimit=[%d],__Gap_Danger_Char_Cnt=[%d],exitCode=[%s]; fixWay: use bigger jsonTxtOut_",jsonTxtLen, jsonTxtOutLimit,__Gap_Danger_Char_Cnt,__debugVar__ErrCode__runtime_c__vars_fn);
    exit(Err01_Beyond_JsonTxtOutLimit);
  }else{
    if(jsonTxtOut_!=NULL && jsonTxtOutLen_!=NULL){
      //复制jsonTxt到jsonTxtOut_
      memcpy(jsonTxtOut_, jsonTxt, jsonTxtLen);
      jsonTxtOut_[jsonTxtLen-1]='\0';
      (*jsonTxtOutLen_)=jsonTxtLen;
    }
  }
//  printf("%s",jsonTxt); //不再输出结果到控制台

}

    list_destroy(_vdLs->_vdVec); // 释放 对象0 : _DEL_0  // delete vdLs->_vdVec;  vdLs->_vdVec= nullptr;
    _DEL_(_vdLs); // delete vdLs;

}

#endif  //__cplusplus判断结束
