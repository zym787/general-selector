#ifndef _VALVE_H_
#define _VALVE_H_

#ifdef _VALVE_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif

//#define SPD_VALVE             28                     // 转阀速度
#define SPD_VALVE           100                     // 转阀速度

#define AXSV                0

#define I_26A               0x00
#define I_22A               0x01
#define I_18A               0x02
#define I_16A               0x03
#define I_05A               0x04

#ifdef A12_909
#define LED_WORK            PCout(15)
#define VALVE_OPT           PAin(15)
#define VALVE_ENA		    PAout(4)
#define VALVE_RST		    PAout(5)
#define VALVE_DIR		    PAout(6)
#define VALVE_CLK		    PAout(7)

#define M_ISET_1           PBout(0)
#define M_ISET_2           PBout(12)
#define M_ISET_3           PAout(11)
#endif
#ifdef A12_906
#define LED_WORK            PCout(14)
#define VALVE_OPT           PCin(15)
#define VALVE_ENA		    PAout(6)
#define VALVE_RST		    PAout(7)
#define VALVE_DIR		    PAout(4)
#define VALVE_CLK		    PAout(5)

#define M_ISET_1           PBout(12)
#define M_ISET_2           PBout(0)
#define M_ISET_3           PAout(11)
#endif
#define ISET(val)   \
    M_ISET_1=(val>>0&0x01);\
    M_ISET_2=(val>>1&0x01);\
    M_ISET_3=(val>>2&0x01);

#ifdef A12_909
#define SCALE               64                      //当前细分数为64
#endif
#ifdef A12_906
#define SCALE               16                      //当前细分数为64
#endif
#define P_ROUND                 200                     //每圈大步数为200

#ifdef A12_909
#define STEPS_1_DEGREE_RD01      (35.6)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD01     (3.6)                  //每0.1度需走的步数为12800/3600=3.555~
#define STEPS_1_DEGREE_RD04      (142.2)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD04     (14.2)                  //每0.1度需走的步数为12800/3600=3.555~
#define STEPS_1_DEGREE_RD10      (355.6)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD10     (35.6)                  //每0.1度需走的步数为12800/3600=3.555~
#define STEPS_1_DEGREE_RD16      (568.9)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD16     (56.9)                  //每0.1度需走的步数为12800/3600=3.555~
#endif
#ifdef A12_906   /* A12_906 16细分 16*200=3200 */
#define STEPS_1_DEGREE_RD01      (8.9)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD01     (0.9)                  //每0.1度需走的步数为12800/3600=3.555~
#define STEPS_1_DEGREE_RD04      (35.6)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD04     (3.6)                  //每0.1度需走的步数为12800/3600=3.555~
#define STEPS_1_DEGREE_RD10      (88.9)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD10     (8.9)                  //每0.1度需走的步数为12800/3600=3.555~
#define STEPS_1_DEGREE_RD16      (142.2)                 //每度需走的步数为12800/360=35.555~
#define STEPS_01_DEGREE_RD16     (14.2)                  //每0.1度需走的步数为12800/3600=3.555~
#endif


enum
{
    VALVE_INITING=0x80,
    VALVE_RUN_END=0x40,
    VALVE_RUNNING=0x08,
    VALVE_ERR=0x20,
};

enum
{
    VALVE_DIR_CW = 0x00,
    VALVE_DIR_CCW = 0x01,
    VALVE_DIR_NER = 0xFF,
};

#define RETRY_TIMES         2                       // 重试次数
#define SIGNAL_SUM          48                      // 码盘检验次数
#define SIGNAL_LIMIT        4                       // 码盘检验次数
#define DEG_DECCEL          40                       // 通道减速区间

#define BYTE_RANGE_MIN      0
#define BYTE_RANGE_MAX      255
#define AGS_ADDR_MIN        0       /* AGS地址最小 0 */
#define AGS_ADDR_MAX        63      /* AGS地址最大 63 */
#define BURN_ADDR           64      /* 老化地址 64 */
#define AGS_ADDR_DEF        1       /* 默认地址 1 */
#define INIT_SPD            15      /* 初始化找位速度 */
#define SPD_MIN             1       /* 最小速度 */
#define SPD_MAX             100      /* 最大速度 */
#define SPD_MIN_RDCR20      8       /* 20减速比 最小速度 */
#define SPD_MAX_RDCR20      35      /* 20减速比 最大速度 */
#define CHANNEL_MIN         3       /* 最小通道数 */
#define CHANNEL_MAX         32      /* 最大通道数 */
#define CHANNEL_DEF         10      /* 默认通道数 */
#define BAUD_MIN            1       /* 最小波特率 */
#define BAUD_MAX            3       /* 最大波特率 */
#define BAUD_DEF            1       /* 默认波特率 */

#define RDCR_1              1       /* 减速比 1 */
#define RDCR_4              4       /* 减速比 4 */
#define RDCR_10             10      /* 减速比 10 */
#define RDCR_16             16      /* 减速比 16 */
#define RDCR_20             20      /* 减速比 20 */

#define PREPORTCNT          4       /* 预输入端口 */
typedef struct
{
    unsigned char Addr;             //模块地址
    unsigned char status;           //通阀当前状态
    unsigned char optBK;
    unsigned char dir;              //寻位方向
    unsigned char retryTms;         //走位重试的次数
    unsigned char bReInit;          // 再次复位转动

    unsigned char initStep;         //阀初始化的步骤
    unsigned char portCur;          //通阀当前位置编号
    unsigned char portDes;          //通阀目标位置编号
    unsigned char direct;           //通阀目标位置编号

    unsigned char dirLast;          //
    unsigned char statusLast;       //
    unsigned char passByOne;        //重新校准1号位标志
    unsigned char bErrRetn;       //
    unsigned char bGetPort;       //
    unsigned char bPassPort;       // pass a channle and display now number
    unsigned char bHalfSeal;        // 是否半通道
    unsigned char bNewInit;         // 刚复位完成
    unsigned short ErrBlinkTime;    //错误灯提示间隔
    unsigned char fixOrg;          	// 原点补偿
    unsigned char spd;          // 再次复位转动

    unsigned int OptStep;           //光耦脉冲步数统计
    unsigned int OptGap;           //光耦高电平脉冲步数
    unsigned int OptBlock;            //光耦低电平脉冲步数
    unsigned int stpCnt;            //初始化后开始补偿的步数

    unsigned int BaudRate;          //运行的波特率值
    unsigned char fDirCw;              // 寻位减速值
    unsigned char fDirCCw;              // 寻位减速值
    unsigned char SnCode[LEN_SN];   // 序列码
    unsigned char serialNum;        // 连续目标孔位，不知道哪个傻逼想的，确定一个孔位要连续分多次走位
    unsigned char serialPort[PREPORTCNT];    // 连续目标孔位，不知道哪个傻逼想的，确定一个孔位要连续分多次走位
}_VALVE_T;
PEXT _VALVE_T Valve;


typedef struct
{
	uint8	rate;
	uint32	stepRound;
	float	stepP1dgr;
	float	stepP01dgr;
}RDC_T;
PEXT RDC_T rdc;

/* 限制值 */
typedef struct
{
    uint8_t spd_min;/* 速度最小值 */
    uint8_t spd_max;/* 速度最大值 */
    uint8_t spd_init;/* 初始化速度 */
} Boundary_T;
PEXT Boundary_T tBoundary;

typedef struct
{
    bool    bEnable;            // retry bit
    bool    typeCtrl;           // type of ctrl shortest coord or define direction
    uint8   dir;                // retry direction
    uint8   times;              // retry times
    uint8   totalSignalCnt;     // total signal count
}_RETRY_T;
PEXT _RETRY_T try;

#define FIX_VAL_LENGHT      14
typedef union
{
    unsigned char array[FIX_VAL_LENGHT];
    struct
    {
        unsigned char port1Val;
        unsigned char port2Val;
        unsigned char port3Val;
        unsigned char port4Val;

        unsigned char port5Val;
        unsigned char port6Val;
        unsigned char port7Val;
        unsigned char port8Val;

        unsigned char port9Val;
        unsigned char port10Val;
        unsigned char port11Val;
        unsigned char port12Val;

        unsigned char dirGap;          //
        unsigned char portCnt;          //
    }fix;
}_12VALVE_FIX;
PEXT _12VALVE_FIX valveFix;

PEXT void ConfigValve(void);
PEXT void InitValve(void);
PEXT void ValveProcess(void);
PEXT void ProcessValve(void);
PEXT void ValveLimitDetect(void);
PEXT void TestBurn(void);

#undef PEXT
#endif


