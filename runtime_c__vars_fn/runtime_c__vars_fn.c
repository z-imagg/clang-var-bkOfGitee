
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
#define Err01_Beyond_JsonTxtOutLimit 11

//危险隔离带长度
#define __Gap_Danger_Char_Cnt 9


/**
 * 关于指针参数, 正常的约定是：
 *   1. 被调用者的入参指针 应该指向一块没有被释放的内存区域
 *   2. 当调用者完成该调用后, 释放刚刚的 入参指针
 * 所以被调用者拿到 入参指针 只能认为其是合法, 因为被调用者没法检测合法性。
 * 假如 被调用者拿到的 入参指针 是 (不合法的)野指针 , 那么责任在 调用者, 被调用者 无责任.
 */

//函数左花括号后 插入 'void* _varLs_ptr=_init_varLs_inFn__RtC00();'
_VarDeclLs *  _init_varLs_inFn__RtC00(char* _srcFilePath, char* _funcName, int funcLBrc_line, int funcLBrc_column){
    //new:HeapObj_vdLs
    _VarDeclLs *vdLs = _NEW_(_VarDeclLs); // 分配 对象0 : _NEW_0  //new _VarDeclLs;
    //new:HeapObj_vdVec
    vdLs->_vdVec=list_new();//new std::vector<_VarDecl>();

    vdLs->srcFilePath=sdsnew(_srcFilePath);
    vdLs->funcName=sdsnew(_funcName);
    vdLs->funcLBrc_line=funcLBrc_line;
    vdLs->funcLBrc_column=funcLBrc_column;

    //依赖模块 runtime_c__TmPnt_ThreadLocal
    vdLs->TmPnt=TL_TmPnt__get();

    return vdLs;
}


//结构体变量声明末尾 插入 'createVar__RtC00(_varLs_ptr,"变量类型名",变量个数);'
void createVar__RtC00(_VarDeclLs *_vdLs, char* _varTypeName, int varSize, short varIsArr, int arrEleSize){

    _VarDecl *vd= _NEW_(_VarDecl);//分配 对象1 : _NEW_1
    vd->varTypeName=sdsnew(_varTypeName);
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
// 术语 jTxtOL 、 XOut_ 见 本函数声明
void destroyVarLs_inFn__RtC00(_VarDeclLs *_vdLs, int jTxtOLimit, char* jsonTxtOut_, int* jTxtOLenOut_){
    list_t* _vdVec = _vdLs->_vdVec; // std::vector<_VarDecl>

  long varDeclCnt = _vdVec->len; //std::distance(_vdVec->begin(), _vdVec->end());

//变量jsonTxt 、变量jsonTxt_vdItem 用于 组装_vdLs为json并输出到控制台
if(varDeclCnt>0){
  sds jsonTxt =  sdsnew("{\"vdLs\":[");

  int varSizeSum=0;
  list_node_t *nodeK;
  list_iterator_t *it = list_iterator_new(_vdVec, LIST_HEAD);
  while ((nodeK = list_iterator_next(it))) {
    _VarDecl* vdK=(_VarDecl*)(nodeK->val); //这不是c++，这是c，无类型信息，只能做危险的强制类型转换
    sds jsonTxt_vdItem   =sdscatprintf(sdsempty(), "{\"varTypeName\":\"%s\", \"varSize\":%d, \"varIsArr\":%d, \"arrEleSize\":%d}", vdK->varTypeName, vdK->varSize, vdK->varIsArr, vdK->arrEleSize);
    // 估计 sdscatprintf自己会释放 其 '第0个参数 sdsempty()'
    jsonTxt=sdscatprintf(jsonTxt,"%s", jsonTxt_vdItem);//现在这是sdscatprintf的 正确语义；原本将 目标参数和来源参数都为jsonTxt 是错误语义（导致指数爆炸增长，最终肯定会缓冲区溢出）
    // 估计 sdscatprintf自己会释放 其 '第0个参数 jsonTxt'
    //若sds类型的格式化串用%S ，则这里sdscatprintf返回NULL， 改为%s则结果正常
    varSizeSum += vdK->varSize;

    //释放本循环内的对象
    sdsfree(jsonTxt_vdItem);//释放sds字符串
    _DEL_(vdK);//释放 对象1 : _DEL_1
  }
  //释放迭代器(不释放链表自身)
  list_iterator_destroy(it);

  jsonTxt=sdscatprintf(jsonTxt,"\"srcFilePath\":\"%s\", \"funcLBrc_line:\":%d, \"funcLBrc_column\":%d, \"varDeclCnt\":%d, \"varSizeSum\":%d\n", _vdLs->srcFilePath , _vdLs->funcLBrc_line , _vdLs->funcLBrc_column , varDeclCnt, varSizeSum ) ;


  int jsonTxtLen=(int)sdslen(jsonTxt);
  // 如果调用者不接收json串，则打印到控制台
  if(jTxtOLimit == 0 && jsonTxtOut_ == NULL && jTxtOLenOut_ == NULL){
    printf("%s",jsonTxt); //不再输出结果到控制台
  }
  // 否则, 如果 看起来具备接收条件 则将json串返回给调用者
  else if(jTxtOLimit > 0 && jsonTxtOut_ != NULL && jTxtOLenOut_ != NULL){
    //[出错,缓冲区尺寸不够] 如果json文本长度 超出 返回缓冲区jsonTxtOut_的尺寸jsonTxtOutLimit ，则直接将错误消息塞入出参。   解决办法是frida使用更大的缓冲区jsonTxtOut_
    if(jsonTxtLen > jTxtOLimit - __Gap_Danger_Char_Cnt){
      //填写错误消息文本给frida，以告知发生了错误
      sprintf(jsonTxtOut_, "[Err01_Beyond_JsonTxtOutLimit] ,jsonTxtLen=[%d],jTxtOLimit=[%d],__Gap_Danger_Char_Cnt=[%d]; fixWay: use bigger jsonTxtOut_\n", jsonTxtLen, jTxtOLimit, __Gap_Danger_Char_Cnt);
      //   填写 缓冲区中字符串实际长度
      (*jTxtOLenOut_)=strlen(jsonTxtOut_);
      //回到函数结尾 ，用以尽快return.   不能直接退出.因为还有要释放的量
      goto _fnEnd;
    }
    //[正常,缓冲区尺寸足够] 否则[如果json文本长度 小于  返回缓冲区jsonTxtOut_的尺寸jsonTxtOutLimit] ，则正常返回json文本
    else{
      //复制jsonTxt到jsonTxtOut_
      //   sds基本路数 是 char们(0结尾) 后跟 描述量 ? 所以可以直接用memcpy?    从sds.c中函数sdscpylen看到的 ， https://github.com/antirez/sds/blob/a9a03bb3304030bb8a93823a9aeb03c157831ba9/sds.c#L427
      //   复制 sds样式量jsonTxt的 char们 到 返回缓冲区jsonTxtOut_
      memcpy(jsonTxtOut_, jsonTxt, jsonTxtLen);
      //   返回缓冲区末尾 加字符串结束标识0
      jsonTxtOut_[jsonTxtLen-1]='\0';
      //   填写 缓冲区中字符串实际长度
      (*jTxtOLenOut_)=jsonTxtLen;
    }
  }

  // 直接return 要换为 'goto _fnEnd' , 理由是 还有量要释放.
  _fnEnd: //函数结尾.
  //释放 此if内的对象
  sdsfree(jsonTxt);
}

//释放此函数内的对象
    list_destroy(_vdLs->_vdVec); // 释放 对象0 : _DEL_0  // delete vdLs->_vdVec;  vdLs->_vdVec= nullptr;
    _DEL_(_vdLs); // delete vdLs;

}

#endif  //__cplusplus判断结束
