#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef _MAIN_H_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif

#define DESCRIPTION         "Selector Valve"
#define CONTROL             "232/485 AGS"
#define SOFTWARE_VERSION    "r13"                /* 软件修改版次 */
#define SOFT_REVISION       (uint16_t)0x0013    /* 软件修改版次 */
#ifndef END_HOLE    /* v2.0.0C 开机1号通 */
#define HOLE_INFO           ">First< Hole"
#define SOFT_NAME           "v2.0.0C"
#define SOFT_VER_NUM        (uint32_t)0x200C0000    /* C 开机1号通 */
#else               /* v2.0.0D 开机末位通 */
#define HOLE_INFO           ">Last< Hole"
#define SOFT_NAME           "v2.0.0D"
#define SOFT_VER_NUM        (uint32_t)0x200D0000    /* D 开机末位通 */
#endif

#define BOARD_0     0x88
#define BOARD_1     0x66
#define SOFT_VER    (SOFT_VER_NUM + SOFT_REVISION)
#define SOFT_VER_C  SOFT_NAME##"-"##SOFTWARE_VERSION
//  v2.0.0r0        2024.07.24  修复半通道起始位错误 (TZY)
//  v2.0.0r2        2024.08.09  保留半通道或1通道 (TZY)
//  v2.0.0r3        2024.10.12  限制繁忙时命令响应 (TZY)
//  v2.0.0r4        2025.01.23  修复最短路径切换指令返回值异常
//  v2.0.0-r5       2025.07.23  修复部分默认参数写入乱码
//                              分离软件版本号中的修改版次和版本名称，修改版本号，支持03读版本
//                              初始化降速增扭
//                              增加超时保护，LED报错快闪
//                              10写临时速度超过30部分会降低一半
//                              默认速度强制设置为20
//                              修复10写临时速度会篡改系统速度，方向仍会被篡改
//  v2.0.0-r6       2025.07.24  恢复10写临时速度，速度不做限制
//                              添加LIMIT_TEMP_SPD宏开关限制临时速度
//  v2.0.0-r7       2025.07.25  查版本可以只管看到版本号，不需要进行进制转换 20 00 00 07
//                              添加09读速度，添加半通道默认关闭、波特率默认9600、CW/CCW默认0
//                              优化modbus协议栈，规范术语，新增0E操作地址错误
//                              添加07读写波特率
//  v2.0.0-r8       2025.08.18  波特率支持38400
//                  2025.08.19  AGS添加99读通道数,修复10写临时速度无法使用方向
//                              06写功能码写通道00,写地址01,复位06,写波特率07,写速度09,序列号08添加写入参数限制
//                              修复串口2,3无法通信问题,修复错误状态无法回复
//  v2.0.0CD-r8     2025.08.19  使用宏开关区分开机末端孔及开机1号孔
//                              屏蔽AGS协议栈及走位调试输出
//                              优化版本号,2.0.0r4->C为开机1号孔,2.0.0r0->D为开机末端孔
//                              读通道改为63操作码
//  v2.0.0CD-r9     2025.08.20  修复下载口参数无法设置38400波特率
//                              写入通道数限制,写入速度限制,写入减速比限制
//  v2.0.0CD-r10    2025.08.21  添加20减速比支持,20减速比速度限制在1-50,正常速度限制1-70
//                              修复AGS地址错误时无法访问问题
//  v2.0.0CD-r11    2025.08.25  修复地址被篡改错误
//  v2.0.0CD-r12    2025.08.26  修复38400波特率打印错误
//  v2.0.0CD-r13    2025.08.27  修复速度范围15-70(20减速比8-35),开机减速使用最小速度,修复20减速比支持,默认减速比10


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
//    uint32_t totalCnt;        /* 切换次数 */
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









