#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef _MAIN_H_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif

// clang-format off

#define DESCRIPTION         "Selector Valve"
#define SOFTWARE_VERSION    "r48"                /* 软件修改版次 */
#define SOFT_REVISION       (uint16_t)0x0048     /* 软件修改版次 */

#define BOARD_0     0x88
#define BOARD_1     0x66
#define SOFT_VER    (SOFT_VER_NUM + SOFT_REVISION)
#ifndef LIMIT_TEMP_SPD
#define SOFT_VER_C  SOFT_NAME##"-"##SOFTWARE_VERSION
#else
#define SOFT_VER_C  SOFT_NAME##"-"##SOFTWARE_VERSION##" "##LTS
#endif
//  v2.0.0r0        2024.07.24  修复半通道起始位错误 (TZY)
//  v2.0.0r2        2024.08.09  保留半通道或1通道 (TZY)
//  v2.0.0r3        2024.10.12  限制繁忙时命令响应 (TZY)
//  v2.0.0r4        2025.01.23  修复最短路径切换指令返回值异常
//  v2.0.0-r5       2025.07.23  修复部分默认参数写入乱码
//                              分离软件版本号中的修改版次和版本名称,修改版本号,支持03读版本
//                              初始化降速增扭
//                              增加超时保护,LED报错快闪
//                              10写临时速度超过30部分会降低一半
//                              默认速度强制设置为20
//                              修复10写临时速度会篡改系统速度,方向仍会被篡改
//  v2.0.0-r6       2025.07.24  恢复10写临时速度,速度不做限制
//                              添加LIMIT_TEMP_SPD宏开关限制临时速度
//  v2.0.0-r7       2025.07.25  查版本可以只管看到版本号,不需要进行进制转换 20 00 00 07
//                              添加09读速度,添加半通道默认关闭、波特率默认9600、CW/CCW默认0
//                              优化modbus协议栈,规范术语,新增0E操作地址错误
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
//  v2.0.0CD-r14    2025.09.17  默认开启限制临时速度,
//                              增加下载口点检指令INSP,会打印出所有参数
//                              修复擦除时默认波特率9600、半通道0、扫描标志问题
//                              修复擦除后复位下载口输出卡死问题
//                              修复AGS协议波特率设置支持,修复HX协议波特率设置,支持38400
//  v2.0.0CD-r15    2025.09.23  默认开启限制临时速度
//                              写入默认参数后锁定驱动,必须再次复位才可生效
//  v2.0.0CD-r16    2025.09.24  修复临时速度判断条件错误
//                              屏蔽未使用的Fix SIG参数
//  v2.0.0CD-r17    2025.10.13  修复老化时无法切换方向问题,现为就近切换通道老化,通道先递增后递减周而复始
//                              增加单次开机老化计数,断电不保存
//  v2.0.0CD-r18    2025.10.13  支援出错重走
//  v2.0.0CD-r19/20    2025.10.14  debug输出,支援扫描时打印脉冲宽度,脉冲容忍度改为20%
//  v2.0.0CD-r21    2025.10.14  修复支援错位后重走
//  v2.0.0CD-r22    2025.10.17  脉冲容忍度改为15%
//  v2.0.0CD-r23    2025.10.17  脉冲容忍度改为10%
//  v2.0.0CD-r24    2025.11.11  修复速度范围,正常速度限制在15-70（减速比10）,其余减速比的速度成倍数关系
//                              新增切换时间输出,在每次走位完成会输出当前位置和切换时间
//  v2.0.0CD-r25    2025.11.14  修复阀错位问题,为无法使用eide编译导致
//  v2.0.0CD-r26    2025.11.14  支持IO,支持中间状态停留一定时间,下载口和AGS可设停留时间
//  v2.0.0CD-r27    2025.11.17  撤销支援错位后重走,支援切换时间记录
//  v2.0.0CD-r28    2025.11.17  合并r22-r26
//  v2.0.0CDE-r29   2025.11.18  分裂C D E F四个版本,C开机1号孔,D开机末端孔,E开机1号孔带IO控制,F开机1号孔多IO控制
//                              汉化部分语句,使用dbg_printf替换调试输出,降低待机电流
//                              支援IO,明确IO使用AGS1.3.1B标准,BI悬空/1 AI悬空/0 BO输出0,下载口IOE使能IO
//                              支援中间状态停留一定时间,下载口和AGS可设停留时间
//                              修复复位标志位,脉冲容忍度还原为10%
//  v2.0.0CDE-r30   2025.11.26  CW/CCW补偿值默认调整为20,汉化部分指令,点检模式支持中文,支持中文显示所有指令,优化中文显示
//                              速度范围改为1和4减速比1-255,10减速比1-100,16 20减速比1-70
//                              删除读取脉冲标志
//  v2.0.0CDE-r31   2025.11.26  修复AGS速度写0时异常,为15RPM
//  v2.0.0CDE-r32   2025.11.28  修复E版本方向错误,切换反向从1到6到5,删除未使用的方向补偿
//  v2.0.0CDE-r33   2025.12.02  修复顺逆时针补偿下载口显示,代码内CW/CCW为电机方向,实际使用为阀头方向,两者相反
//  v2.0.0CDE-r34   2025.12.09  老化次数支持断电保存,下载口TESTC读写老化次数
//  v2.0.0CDEF-r35  2025.12.11  添加标准库ADC,GPIO,RCC支援
//                  2025.12.12  F版本, 开机1号孔多IO控制,完成ADC移植
//  v2.0.0CDEF-r36  2025.12.12  F版本,完成IO检测及板宏定义,支持条件编译
//                              请注意,926电路板施密特为反向处理,故IO输入为低电平有效,为保持一致输出也为低电平有效
//                              IN1     IN2     状态    OUT1    OUT2    FBOUT
//                              1/悬空  1/悬空  1       1       1       完成时为0,否则1
//                              0       1/悬空  2       0       1       ERROUT
//                              1/悬空  0       3       1       0       保持时为0,否则1
//                              0       0       4       0       0       报错时,OUT不变
//  v2.0.0CDEF-r37  2025.12.15  F版本默认12通,指定顺序切换1-12-11-10
//  v2.0.0CDEF-r38  2025.12.16  F版本默认12通,指定顺序切换1-2-3-4
//  v2.0.0CDEF-r39  2025.12.16  F版本下载口STATC写入切换顺序,默认12通切换顺序1-2-3-4,修复打印切换顺序错误
//  v2.0.0CDEF-r40  2025.12.16  F修复引脚初始化，错位时OUT1和OUT2改为不变，开机增加切换顺序检查和电路板检查
//  v2.0.0CDEF-r41  2025.12.16  F修复引脚初始化
//  v2.0.0CDEF-r42  2025.12.16  F修复输入引脚持续为高问题
//  v2.0.0CDEF-r43  2026.03.09  串口新增0D功能码读写半通道功能,点检模式支援波特率值显示
//  v2.0.0CDEF-r44  2026.03.09  新增切换次数记录,下载口MOVES/串口0A功能码读写切换次数
//                  2026.03.30  优化参数,新增log模块,重构AGS协议,读指令添加长度限制
//  v2.0.0CDEF-r45  2026.04.01  新增modbus支援,实现03功能码
//  v2.0.0CDEF-r45  2026.04.02  回滚,读指令添加长度限制
//  v2.0.0CDEF-r46  2026.04.08  新增modbus支援,实现03/06功能码,支援广播地址0x00
//  v2.0.0CDEF-r47  2026.04.08  去除老化次数显示,防呆设计
//  v2.0.0CDEF-r48  2026.05.19  F版本修复IO初始化,以防止出现IO无法输入/输出问题
//                              同步QHF Modbus协议,添加参数限幅



//----EEPROM存储地址分配---//
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

#define ADDR_RDC_RATE           (ADDR_DIR_SD+LEN_DIR_SD)
#define LEN_RDC_RATE            1

#define ADDR_HALF_SEAL          (ADDR_RDC_RATE+LEN_RDC_RATE)
#define LEN_HALF_SEAL           1

#define ADDR_IO_CTRL            (ADDR_HALF_SEAL+LEN_HALF_SEAL)
#define LEN_IO_CTRL             1

#define ADDR_PAUSE_TIME         (ADDR_IO_CTRL+LEN_IO_CTRL)
#define LEN_PAUSE_TIME          4

#define ADDR_BURN_CNT           (ADDR_PAUSE_TIME+LEN_PAUSE_TIME)
#define LEN_BURN_CNT            2

#define ADDR_STATE_CHANNEL      (ADDR_BURN_CNT+LEN_BURN_CNT)
#define LEN_STATE_CHANNEL       4

#define ADDR_TOTAL_CNT          (ADDR_STATE_CHANNEL+LEN_STATE_CHANNEL)
#define LEN_TOTAL_CNT           4

#define ADDR_GOD_MODE           (ADDR_TOTAL_CNT+LEN_TOTAL_CNT)
#define LEN_GOD_MODE            1

//------------------------------------------------------------------------------------------------------------

// clang-format on

#define NORMAL_BLINK   1500  // 正常运行的闪烁间隔
#define RETRY_TIME_OUT 400   // 异常运行的闪烁间隔

#define KEY     PBin(5)
#define RX_EN() (PBout(1) = 0)
#define TX_EN() (PBout(1) = 1)

/**
 * @brief     : 控制协议枚举
 */
typedef enum PROTOCOL {
        AGS_MODBUS, /* AGS协议 基于Modbus魔改 */
        EXT_COMM,   /* HX协议 帧头+帧尾 */
        MODBUS,     /* Modbus协议 */

        PROTOCOL_NUM
} Protocol_T;

/**
 * @brief     : 波特率枚举
 */
typedef enum BAUDRATETYPE {
        BAUD_NONE = 0u,
        BAUD_9600 = 1u,
        BAUD_19200 = 2u,
        BAUD_38400 = 3u,

        BAUD_NUM
} BaudRate_T;

typedef enum GODMODE {
        GD_NORMAL = 0u,
        GD_FACTORY = 1u,
        GD_AGING = 2u,
} GodMode_T;

extern uint16_t BaudRate_V[BAUD_NUM];
extern uint16_t BaudRate_Time[BAUD_NUM];

typedef struct {
        /* 系统参数 */
        Protocol_T protocol_type; /* 协议 */
        BaudRate_T baudrate;      /* 波特率 */
        bool bRdPulse;           /* ?读取脉冲标志 */
        uint32_t OptBlockLast;
        uint32_t protectTimeOut;
        uint32_t burnCnt;       /* 烧机次数 */
        bool bCountLastTime;    /* 是否记录切换时间 */
        uint32_t lastTime;      /* 切换时间 */
        uint32_t timeRamp[3];   /* 切换时间序列 */
        uint8_t recordTimeRamp; /* 记录切换时间 */
        bool ioCtrl;            /* IO控制位 */
        uint32_t pauseTime;     /* 停留时间 */
        bool ctrlPause;         /* 停留时间控制 */
        uint32_t totalCnt;      /* 切换次数 */
        uint32_t totalCntLst;
        GodMode_T GodMode;      /* 当前模式 */
} _SYS_T;
PEXT _SYS_T syspara;

PEXT uint8_t intCtrl;

PEXT void ParameterInit(void);
PEXT int main(void);
PEXT void DebugOut(void);
PEXT void UsrLimit(void);
PEXT void ErrBlink(void);
PEXT void errProcRun(void);

#undef PEXT
#endif
