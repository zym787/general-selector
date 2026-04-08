#ifndef _PROTEXT_H_
#define _PROTEXT_H_

#ifdef _PROTEXT_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif

enum COMM
{
    HEAD_BYTE           =0xAA,  //0x5B
    LEN_DATA            =4,     //0x5B
    PROTOCOL_HEAD		=0,	    //协议头
    PROTOCOL_ADDR	    =1,	    //协议命令
    PROTOCOL_COMMAND	=2,	    //协议命令
    PROTOCOL_DATA		=3,	    //协议数据
    PROTOCOL_CS 		=4,	    //协议校验低字节
    PROTOCOL_OK		    =5,	    //协议尾
};


enum CMD
{
    SHIFT_NML           =0x01,
    SHIFT_DE            =0x02,
    SHIFT_IN            =0x03,
    ACTN_RESET          =0x05,
    WR_CUST_DATA        =0X7F,
    RD_CUST_DATA        =0XAF,
    RD_NOW_STA          =0X90,
    RD_NOW_POS          =0X99,
    SET_PROTOCAL        =0XFF,
};

#define RECEIVE_LENS        8      //带数据返回长度
#define REPLY_LENS          7      //带数据返回长度

#define rstUartBuf()        protext.rxCount = 0 
#define PROT_HEAD 			protext.usartBuf[0]  		//命令编号
#define P_ADDR 			    protext.usartBuf[1]  		//命令编号
#define P_COMMAND 			protext.usartBuf[2]  		//命令编号


typedef struct 
{
    uint16_t  dataLen;
    uint8_t   time;
    uint8_t   stepCnt;
    uint8_t   rxCount;
    uint8_t   rxTimeOn;
    uint8_t   rxTimeCnt;
    uint8_t   f_RxErr;
    uint8_t   usartBuf[128];
    uint8_t   replyBuf[128];
}_PROTEXT_T;
PEXT _PROTEXT_T protext;


PEXT void RxUsart(uint8_t res);
PEXT void UsartProcess(void);
PEXT void CommInit(void);


#undef PEXT
#endif


