#ifndef __MODBUS_H__
#define __MODBUS_H__

/* 包含头文件 ----------------------------------------------------------------*/
// #include "stm32f1xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
/**
 * @brief    Modbus结构体
 */
typedef struct {
        __IO uint8_t RunState;    /* 总线运行状态 */
        __IO uint8_t ErrorState;  /* 总线错误状态 */
        __IO uint16_t times;      /* 单帧接收超时时间 */
        __IO uint8_t ReciveCount; /* 接收数据长度 */
        __IO uint8_t Address;     /* 从站地址 */
} Modbus_T;


typedef struct {
        uint16_t IN1;

} REG_VALUE;

/* 宏定义 --------------------------------------------------------------------*/
#define DEBUG_MODBUS          (0)       /* 是否启用调试输出 */
#define SUPPORT_FUN_CODE_01H  (0)       /* 是否启用功能码01H */
#define SUPPORT_FUN_CODE_02H  (0)       /* 是否启用功能码02H */
#define SUPPORT_FUN_CODE_03H  (1)       /* 是否启用功能码03H */
#define SUPPORT_FUN_CODE_04H  (0)       /* 是否启用功能码04H */
#define SUPPORT_FUN_CODE_05H  (0)       /* 是否启用功能码05H */
#define SUPPORT_FUN_CODE_06H  (1)       /* 是否启用功能码06H */
#define SUPPORT_FUN_CODE_10H  (1)       /* 是否启用功能码10H */

#define MB_SLAVEADDR    0x0001
#define MB_ALLSLAVEADDR 0x00FF

#define FUN_CODE_01H 0x01  // 功能码01H
#define FUN_CODE_02H 0x02  // 功能码02H
#define FUN_CODE_03H 0x03  // 功能码03H
#define FUN_CODE_04H 0x04  // 功能码04H
#define FUN_CODE_05H 0x05  // 功能码05H
#define FUN_CODE_06H 0x06  // 功能码06H
#define FUN_CODE_10H 0x10  // 功能码10H

/* 本例程所支持的功能码,需要添加新功能码还需要在.c文件里面添加 */
#define IS_NOT_FUNCODE(code)                                                                                       \
        (!((code == FUN_CODE_01H) || (code == FUN_CODE_02H) || (code == FUN_CODE_03H) || (code == FUN_CODE_04H) || \
           (code == FUN_CODE_05H) || (code == FUN_CODE_06H) || (code == FUN_CODE_10H)))

#define EX_CODE_NONE 0x00  // 异常码 无异常
#define EX_CODE_01H  0x01  // 异常码
#define EX_CODE_02H  0x02  // 异常码
#define EX_CODE_03H  0x03  // 异常码
#define EX_CODE_04H  0x04  // 异常码

/* 寄存器地址定义 */
#define AP_PARAM_START                     0ul
#define AP_PARAM_NUM                       100ul
// 控制指令
#define AP_PARAM_R_CTRL_SET_NORMAL         0ul  // 设置切换通道
#define AP_PARAM_R_CTRL_SET_CW             1ul  // 设置切换通道[顺时针]
#define AP_PARAM_R_CTRL_SET_CCW            2ul  // 设置切换通道[逆时针]
#define AP_PARAM_R_CTRL_SET_FREE           3ul
#define AP_PARAM_R_CTRL_REG_4              4ul  // 老化测试功能使能
#define AP_PARAM_R_CTRL_SET_ZERO           5ul  //
#define AP_PARAM_R_CTRL_SET_BREAK          6ul  //
#define AP_PARAM_R_CTRL_SET_RESET_ERR      7ul  //
#define AP_PARAM_R_CTRL_REG_8              8ul
#define AP_PARAM_R_CTRL_COMMAND            9ul
// 状态参数查询
#define AP_PARAM_R_CHANNEL_CUR             10ul  // 当前通道
#define AP_PARAM_R_CONTROL_STATE           11ul  // 控制状态
#define AP_PARAM_R_BCKUP_REG_12            12ul  // PWM有效宽度
#define AP_PARAM_R_BCKUP_REG_13            13ul  // mt6816校准的角度数据
#define AP_PARAM_R_BCKUP_REG_14            14ul  // 目标位置
#define AP_PARAM_R_BCKUP_REG_15            15ul  // 目标位置(由信号输入)
#define AP_PARAM_R_BCKUP_REG_16            16ul  //
#define AP_PARAM_R_BCKUP_REG_17            17ul  //
#define AP_PARAM_R_BCKUP_REG_18            18ul
#define AP_PARAM_R_SOFTWARE                19ul  // 软件版本号
// 用户存储空间
#define AP_PARAM_R_USER_00                 20ul
#define AP_PARAM_R_USER_01                 21ul
#define AP_PARAM_R_USER_02                 22ul
#define AP_PARAM_R_USER_03                 23ul
#define AP_PARAM_R_USER_04                 24ul
#define AP_PARAM_R_USER_05                 25ul
#define AP_PARAM_R_USER_06                 26ul
#define AP_PARAM_R_USER_07                 27ul
#define AP_PARAM_R_USER_08                 28ul
#define AP_PARAM_R_USER_09                 29ul
// 用户备用空间
#define AP_PARAM_R_BCKUP_REG_30            30ul
#define AP_PARAM_R_BCKUP_REG_31            31ul
#define AP_PARAM_R_BCKUP_REG_32            32ul
#define AP_PARAM_R_BCKUP_REG_33            33ul
#define AP_PARAM_R_BCKUP_REG_34            34ul
#define AP_PARAM_R_BCKUP_REG_35            35ul
#define AP_PARAM_R_BCKUP_REG_36            36ul
#define AP_PARAM_R_BCKUP_REG_37            37ul
#define AP_PARAM_R_BCKUP_REG_38            38ul
#define AP_PARAM_R_BCKUP_REG_39            39ul
#define AP_PARAM_R_BCKUP_REG_40            40ul
#define AP_PARAM_R_BCKUP_REG_41            41ul
#define AP_PARAM_R_BCKUP_REG_42            42ul
#define AP_PARAM_R_BCKUP_REG_43            43ul
#define AP_PARAM_R_BCKUP_REG_44            44ul
#define AP_PARAM_R_BCKUP_REG_45            45ul
#define AP_PARAM_R_BCKUP_REG_46            46ul
#define AP_PARAM_R_BCKUP_REG_47            47ul
#define AP_PARAM_R_BCKUP_REG_48            48ul
#define AP_PARAM_R_BCKUP_REG_49            49ul
// 通讯及相关系统参数
#define AP_PARAM_RW_MODBUS_BAUDRATE        50ul  // 485通讯速率
#define AP_PARAM_RW_MODBUS_ADDR            51ul  // 485设备ID
#define AP_PARAM_RW_MOVE_HALF_SETUP_NEABLE 52ul  // 开机切换至半通道使能位
#define AP_PARAM_RW_MOVE_PROX_ENABLE       53ul  // 切换就近通道使能位
#define AP_PARAM_RW_CAN_BAUDRATE           54ul  // CAN通讯速率
#define AP_PARAM_RW_CAN_ADDR               55ul  // CAN设备ID
#define AP_PARAM_RW_MOTOR_CTRL_WAITTIME    56ul  // 连续通道切换等待时间
#define AP_PARAM_RW_MOTOR_CTRL_MODE        57ul
#define AP_PARAM_RW_CTRL_AGING             58ul  // 老化测试指令
#define AP_PARAM_RW_BCKUP_REG_59           59ul
// 用户存储空间
#define AP_PARAM_RW_USER_00                60ul
#define AP_PARAM_RW_USER_01                61ul
#define AP_PARAM_RW_USER_02                62ul
#define AP_PARAM_RW_USER_03                63ul
#define AP_PARAM_RW_USER_04                64ul
#define AP_PARAM_RW_USER_05                65ul
#define AP_PARAM_RW_USER_06                66ul
#define AP_PARAM_RW_USER_07                67ul
#define AP_PARAM_RW_USER_08                68ul
#define AP_PARAM_RW_USER_09                69ul
// 用户备用空间
#define AP_PARAM_RW_BCKUP_REG_70           70ul
#define AP_PARAM_RW_BCKUP_REG_71           71ul
#define AP_PARAM_RW_BCKUP_REG_72           72ul
#define AP_PARAM_RW_BCKUP_REG_73           73ul
#define AP_PARAM_RW_BCKUP_REG_74           74ul
#define AP_PARAM_RW_BCKUP_REG_75           75ul
#define AP_PARAM_RW_BCKUP_REG_76           76ul
#define AP_PARAM_RW_BCKUP_REG_77           77ul
#define AP_PARAM_RW_BCKUP_REG_78           78ul
#define AP_PARAM_RW_BCKUP_REG_79           79ul
#define AP_PARAM_RW_BCKUP_REG_80           80ul
#define AP_PARAM_RW_BCKUP_REG_81           81ul
#define AP_PARAM_RW_BCKUP_REG_82           82ul
#define AP_PARAM_RW_BCKUP_REG_83           83ul
#define AP_PARAM_RW_OPTO_PERIOD            84ul  // 脉冲周期值
#define AP_PARAM_RW_OPTO_WIDTH_MIN         85ul  // 脉宽最小值
#define AP_PARAM_RW_OPTO_WIDTH_MAX         86ul  // 脉宽最大值
#define AP_PARAM_RW_OPTO_CALI_SPEED        87ul  // 校准速度值
#define AP_PARAM_RW_VALVE_DIR_GAP          88ul
#define AP_PARAM_RW_BCKUP_REG_89           89ul
// 电机控制参数
#define AP_PARAM_RW_COMMAND                90ul  // 设置密码
#define AP_PARAM_RW_RATED_SPEED            91ul  // 运行最大速度(转/秒)
#define AP_PARAM_RW_RATED_UP_ACC           92ul  // 运行最大加速度(r/ss)
#define AP_PARAM_RW_RATED_DOWN_ACC         93ul  // 运行最大减速度(r/ss)
#define AP_PARAM_RW_RATED_CURRENT          94ul  // 额定电流
#define AP_PARAM_RW_CALI_CURRENT           95ul  // 校准电流
#define AP_PARAM_RW_CHANNEL_MAX            96ul  // 最大通道数
#define AP_PARAM_RW_MOTOR_TYPE             97ul  // 电机类型: 1->选择阀 2->切换阀
#define AP_PARAM_RW_COMPEN                 98ul  // 原点补偿值(单位: 0.1度)
#define AP_PARAM_RW_DEC_RATIO              99ul  // 减速比

/* 扩展变量 ------------------------------------------------------------------*/
extern Modbus_T modbus;
extern uint8_t Rx_Buffer[LENGTH_MB_DATA];
extern uint8_t Tx_Buffer[LENGTH_MB_DATA];

/* 函数声明 ------------------------------------------------------------------*/
void mb_Init(void);
void mb_TimesProcess(void);
void mb_Send(uint8_t _length);
void mb_Receive(uint8_t _recStr);
void mb_Error(void);
#if (SUPPORT_FUN_CODE_03H != 0)
void mb_03H(void);
#endif
#if (SUPPORT_FUN_CODE_06H != 0)
void mb_06H(void);
#endif
#if (SUPPORT_FUN_CODE_10H != 0)
void mb_10H(void);
#endif
void mb_Poll(void);

#endif /* __MODBUS_H__ */
