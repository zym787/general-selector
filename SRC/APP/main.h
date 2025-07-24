#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef _MAIN_H_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif

#define DESCRIPTION         "Selector Valve"
#define SOFT_VER            2006        /* 软件版本4AGS */
#define SOFTWARE_VERSION    "r6"        /* 软件修改版次 */
#define SOFT_NAME           "v2.0.0-"
#define SOFT_VER_C  SOFT_NAME##""##SOFTWARE_VERSION
//  v2.0.0r0    2024.07.24  修复半通道起始位错误 (TZY)
//  v2.0.0r2    2024.08.09  保留半通道或1通道 (TZY)
//  v2.0.0r3    2024.10.12  限制繁忙时命令响应 (TZY)
//  v2.0.0r4    2025.01.23  修复最短路径切换指令返回值异常
//  v2.0.0-r5   2025.07.23  修复部分默认参数写入乱码
//                          分离软件版本号中的修改版次和版本名称，修改版本号，支持03读版本
//                          初始化降速增扭
//                          增加超时保护，LED报错快闪
//                          10写临时速度超过30部分会降低一半
//                          默认速度强制设置为20
//                          修复10写临时速度会篡改系统速度，方向仍会被篡改
//  v2.0.0-r6   2025.07.24  恢复10写临时速度，速度不做限制
//                          添加LIMIT_TEMP_SPD宏开关限制临时速度

//------------------------------------------------------------------------------------------------------------
#define ADDR_BOARD_ID           0
#define LEN_BOARD_ID            2

#define ADDR_MODULE_NUM         (ADDR_BOARD_ID+LEN_BOARD_ID)
#define LEN_MODULE_NUM          1

#define ADDR_VALVE_FIX          (ADDR_MODULE_NUM+LEN_MODULE_NUM)
#define LEN_VALVE_FIX           1

#define ADDR_DIR_FIX            (ADDR_VALVE_FIX+LEN_VALVE_FIX)
#define LEN_DIR_FIX             1

#define ADDR_PORT_CNT           (ADDR_DIR_FIX+LEN_DIR_FIX)
#define LEN_PORT_CNT            1

#define ADDR_INTVL              (ADDR_PORT_CNT+LEN_PORT_CNT)
#define LEN_INTVL               1

#define ADDR_SPD                (ADDR_INTVL+LEN_INTVL)
#define LEN_SPD                 1

#define ADDR_SN                 (ADDR_SPD+LEN_SPD)
#define LEN_SN                  5

#define ADDR_PROTOCAL           (ADDR_SN+LEN_SN)
#define LEN_PROTOCAL            1

#define ADDR_BAUD               (ADDR_PROTOCAL+LEN_PROTOCAL)
#define LEN_BAUD                1

#define ADDR_RDP               	(ADDR_BAUD+LEN_BAUD)
#define LEN_RDP                	1

#define ADDR_SIG               	(ADDR_RDP+LEN_RDP)
#define LEN_SIG                	32

#define ADDR_SYMBOL            	(ADDR_SIG+LEN_SIG)
#define LEN_SYMBOL            	8

#define ADDR_DIR_SD             (ADDR_SYMBOL+LEN_SYMBOL)
#define LEN_DIR_SD              2

#define ADDR_RDC_RATE			(ADDR_DIR_SD+LEN_DIR_SD)
#define LEN_RDC_RATE			1

#define ADDR_HALF_SEAL			(ADDR_RDC_RATE+LEN_RDC_RATE)
#define LEN_HALF_SEAL			1
//------------------------------------------------------------------------------------------------------------

#define NORMAL_BLINK            1500       //正常运行的闪烁间隔
#define RETRY_TIME_OUT          400        //异常运行的闪烁间隔

#define	KEY		    PBin(5)
#define	RX_EN()		(PBout(1)=0)
#define	TX_EN()		(PBout(1)=1)

enum PROTOCAL
{
    MY_MODBUS,
    EXT_COMM
};

typedef struct
{
    uint8   typeProtocal;
    uint8   bdrate;
    bool    bRdPulse;
    uint32  OptBlockLast;
    uint32_t protectTimeOut;
}_SYS_T;
PEXT _SYS_T syspara;

PEXT uint8 intCtrl;

PEXT void ParameterInit(void);
PEXT int main(void);
PEXT void DebugOut(void);
PEXT void UsrLimit(void);
PEXT void ErrBlink(void);


#undef PEXT
#endif









