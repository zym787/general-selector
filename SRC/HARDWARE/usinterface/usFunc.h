#ifndef _USFUNC_H_
#define _USFUNC_H_

#ifdef _USFUNC_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif

//用于批量注释掉调试输出语句
//"..."代表一个可以变化的参数表。使用保留名"__VA_ARGS__"把参数传递给宏。
//当宏的调用展开时，实际的参数就传递给 printf()了。
#define FSW_DBG_PRINT       1           //输出信息开关
#define FSW_WORK_PRINT      1           //输出信息开关

#if FSW_DBG_PRINT
#define LV_NA                   0
#define LV_INFO                 1
#define LV_DBG                  2
#define	printd(fmt, ...)		printf(fmt, ##__VA_ARGS__)
#define prInfo(lv,fmt, ...)     do{if(lv>=LV_INFO) printf(fmt, ##__VA_ARGS__);}while(0)
#define prDbg(lv,fmt, ...)      do{if(lv>=LV_DBG) printf(fmt, ##__VA_ARGS__);}while(0)
#else
#define printd(...)
#endif

#if FSW_WORK_PRINT
#define	printw(fmt, ... )		printf(fmt, ##__VA_ARGS__)
#else
#define printw(...)
#endif

#define S_LIST_M    "\r ---------------------Command Line----------------------\
                    \r|-/?        显示所有命令                                |\
                    \r|-VR        显示软件版本                                |\
                    \r|-IIC       擦除EEPROM                                  |\
                    \r|-POS       移动到指定位置                              |\
                    \r|-ADDR      设置/地址                                   |\
                    \r|-INT       设置/显示间隔                               |\
                    \r|-SPD       设置/显示速度                               |\
                    \r|-SN        设置/显示序列号                             |\
                    \r|-PRTCL     设置/显示协议                               |\
                    \r|-BDR       设置/显示波特率                             |\
                    \r|-SCAN      设置/显示间隔                               |\
                    \r|-DIRCW     设置/显示顺时针补偿                         |\
                    \r|-DIRCCW    设置/显示逆时针补偿                         |\
                    \r|-RESET     复位                                        |\
                    \r|-CNT       设置/显示通道数                             |\
                    \r|-RDCR      设置/显示减速比                             |\
                    \r|-HALF      设置/显示半通道                             |\
                    \r|-INSP      点检模式                                    |\
                    \r|-PAUSET    设置/显示停留时间                           |\
                    \r|-IOE       IO输出引脚翻转                              |\
                    \r -------------------------------------------------------\
                    "

#define S_LIST_SH   "\r --------------------Sub command Line-------------------"
#define S_LIST_SBD  "\r --------------------Sub command Body-------------------"
#define S_LIST_SE   "\r                         Yes  No                        \
                     \r -------------------------------------------------------"

#define READ_ACT            0
#define WRITE_ACT           1

#define FLASH_CHK           1                       //IIC-1 flash-0

//定义结构体,包含一个步骤,一个函数指针
typedef struct
{
    unsigned char StateIndex;                       //当前步骤索引号
    void (*CurrOp)(char);                           //当前状态应该执行的功能操作
}_TAB_T;


#define	CMD_VR   	        "VR"
#define	CMD_LIST	        "/?"
#define	CMD_MAP	            "MAP"
#define	CMD_IIC   	        "IIC"
#define	CMD_TS	            "TS"            //VALVE PORT
#define	CMD_CAT	            "CAT"            //VALVE PORT
#define	CMD_FET	            "FET"            //VALVE PORT
#define	CMD_POS	            "POS"            //VALVE PORT
#define	CMD_FIX	            "FIX"            //VALVE PORT
#define	CMD_ADDR	        "ADDR"            //VALVE PORT

PEXT void ChRUN(char *cmdName);

#undef PEXT
#endif


