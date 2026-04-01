/* 包含头文件 ----------------------------------------------------------------*/
#include "common.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
ELAB_TAG("modbus"); /* elog 标签 */
/* 私有变量 ------------------------------------------------------------------*/
REG_VALUE R_value;
Modbus_T modbus;
uint8_t Rx_Buffer[LENGTH_MB_DATA];
uint8_t Tx_Buffer[LENGTH_MB_DATA];


/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
#if (SUPPORT_FUN_CODE_01H != 0)
static uint16_t MB_RSP_01H(uint16_t _TxCount, uint16_t _AddrOffset, uint16_t _CoilNum);
#endif
#if (SUPPORT_FUN_CODE_02H != 0)
static uint16_t MB_RSP_02H(uint16_t _TxCount, uint16_t _AddrOffset, uint16_t _CoilNum);
#endif
#if (SUPPORT_FUN_CODE_03H != 0)
static uint8_t MB_RSP_03H(uint16_t _TxCount, uint16_t *_AddrOffset, uint16_t _RegNum);
#endif
#if (SUPPORT_FUN_CODE_04H != 0)
static uint8_t MB_RSP_04H(uint16_t _TxCount, uint16_t _AddrOffset, uint16_t _RegNum);
#endif
#if (SUPPORT_FUN_CODE_05H != 0)
static uint8_t MB_RSP_05H(uint16_t _TxCount, uint16_t _AddrOffset, uint16_t _RegDATA);
#endif
#if (SUPPORT_FUN_CODE_06H != 0)
static uint8_t MB_RSP_06H(uint16_t _TxCount, uint16_t _AddrOffset, uint16_t _RegNum, uint16_t *_AddrAbs);
#endif
#if (SUPPORT_FUN_CODE_10H != 0)
static uint8_t MB_RSP_10H(uint16_t _TxCount, uint16_t _AddrOffset, uint16_t _RegNum, uint16_t *_AddrAbs,
                          uint8_t *_Datebuf);
#endif

/* 函数体 --------------------------------------------------------------------*/

/**
 * @brief     : 初始化Modbus协议
 */
void mb_Init(void)
{
        RX_EN(); /* 开机为接收模式 */
        I2CPageRead_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.baudrate);
        if (BAUD_38400 < syspara.baudrate) {
                syspara.baudrate = BAUD_9600; /* 9600 */
        }
        Usart2_Init(36, BaudRate_V[syspara.baudrate]); /* UART2 19200bps */
        delay_ms(100);
        Usart3_Init(36, BaudRate_V[syspara.baudrate]); /* UART3 19200bps */
        delay_ms(100);
        TIM3_Init(BaudRate_Time[syspara.baudrate], 71);  // 45us--0.45ms
        delay_ms(100);

        elog_debug("USART2/3 Init, baudrate: %d", BaudRate_V[syspara.baudrate]);

        /* 参数配置 */
        modbus.RunState = MB_IDLE;
        modbus.ErrorState = ERR_NOT;
        modbus.times = 0;
        modbus.ReciveCount = 0;
        modbus.Address = 1;

        memset(Rx_Buffer, 0, sizeof(Rx_Buffer));
        memset(Tx_Buffer, 0, sizeof(Tx_Buffer));
}

/**
 * @brief    在定时器中断中定时调用,用于处理超时检测
 */
void mb_TimesProcess(void)
{
        if (modbus.RunState & MB_BUSY) {
                /* 总线检测 */
                if (modbus.times < BUS_IDLE_TIME) {
                        modbus.times++;
                } else {
                        /* 总线进入空闲,帧结束或开始 */
                        if (modbus.RunState == MB_RECIVE_ERR) {
                                /* 接收过程中,有出现数据存储空间溢出或间隔时间超过T1.5 */
                                modbus.ErrorState = ERR_MB_DEVICE;
                                modbus.RunState = MB_IDLE;
                        } else if (modbus.RunState == MB_NO_RESPONSE) {
                                modbus.RunState = MB_IDLE;
                        } else if (modbus.RunState == MB_RECIVE) {
                                modbus.RunState = MB_RECIVE_END;
                        }
                }
        }
}

/**
 * @brief     : 发送数据
 * @param    _length
 */
void mb_Send(uint8_t _length)
{
        elog_debug("%s(): %d", __FUNCTION__, _length);
        XF_LOG_BUFFER_HEX(Tx_Buffer, _length);
        TX_EN();
        if (_length) {
                modbus.RunState = MB_SEND;
                for (uint8_t cnt = 0; cnt < _length; cnt++) {
                        while ((USART3->SR & 0x40) == 0)
                                ; /* 等待发送结束 */
                        USART3->DR = Tx_Buffer[cnt];
                        modbus.times = 0; /* 重新计时 */
                }
        }
        while ((USART3->SR & 0x40) == 0)
                ; /* 等待发送结束 */
        RX_EN();

        if (_length) {
                modbus.RunState = MB_SEND;
                for (uint8_t cnt = 0; cnt < _length; cnt++) {
                        while ((USART2->SR & 0x40) == 0)
                                ; /* 等待发送结束 */
                        USART2->DR = Tx_Buffer[cnt];
                        modbus.times = 0; /* 重新计时 */
                }
        }
        while ((USART2->SR & 0x40) == 0)
                ; /* 等待发送结束 */
        modbus.RunState = MB_IDLE;
        modbus.ReciveCount = 0;
}

/**
 * @brief     : 接收数据
 * @param    _recStr
 */
void mb_Receive(uint8_t _recStr)
{
        modbus.times = 0; /* 重新计时 */
        if (modbus.RunState == MB_IDLE && !modbus.ReciveCount) {
                /* 空闲并且数据处理结束,可以进行新的接收 */
                if (modbus.Address == _recStr || MB_Broadcast_ADDR == _recStr) {
                        modbus.RunState = MB_RECIVE;
                        modbus.ErrorState = ERR_NOT;
                        modbus.ReciveCount = 1;
                        Rx_Buffer[0] = _recStr;
                } else {
                        /* 非本设备地址,或非当前查询的设备,且非广播地址*/
                        modbus.RunState = MB_NO_RESPONSE;
                        modbus.ErrorState = ERR_MB_DEVICE;
						elog_error("%d Address Error", _recStr);
                }
        } else if (modbus.RunState == MB_RECIVE) {
                /* 如果溢出或者传输过程出现时间间隔超过T1.5,都不再接收*/
                if (modbus.ReciveCount < LENGTH_MB_DATA && modbus.times < FRAME_ERR_TIME) {
                        Rx_Buffer[modbus.ReciveCount] = _recStr;
                } else {
                        modbus.RunState = MB_RECIVE_ERR;
						elog_error("%d Frame Error", _recStr);
                }
                ++modbus.ReciveCount;
        }
}

/**
 * @brief    处理错误
 */
void mb_Error(void)
{
        uint16_t crc16;

        if (modbus.ErrorState == ERR_MB_FUN || modbus.ErrorState == ERR_MB_ADDR ||
            modbus.ErrorState == ERR_MB_DATA) {
                /* 从模式,发送响应数据 */
                Tx_Buffer[0] = Rx_Buffer[0];
                Tx_Buffer[1] = Rx_Buffer[1] | 0x80;
                Tx_Buffer[2] = modbus.ErrorState;
                crc16 = ModbusCRC16((uint8_t *)&Tx_Buffer, 3);
                Tx_Buffer[3] = crc16 >> 8;
                Tx_Buffer[4] = crc16 & 0xFF;
                if (Rx_Buffer[0] != MB_Broadcast_ADDR) {
                        mb_Send(5);
                }
                elog_error("Error %d", modbus.ErrorState);
        }
        modbus.ErrorState = ERR_NOT;
}

/**
 * @brief     : 03 读保持寄存器
 */
void mb_03H(void)
{
        /*
         从机地址为01H。保持寄存器的起始地址为0010H，结束地址为0011H。该次查询总共访问2个保持寄存器。
         主机发送:
                 01 从机地址
                 03 功能码
                 00 寄存器地址高字节
                 10 寄存器地址低字节
                 00 寄存器数量高字节
                 02 寄存器数量低字节
                 C5 CRC高字节
                 CE CRC低字节

         从机应答: 	保持寄存器的长度为2个字节。对于单个保持寄存器而言，寄存器高字节数据先被传输，
                                 低字节数据后被传输。保持寄存器之间，低地址寄存器先被传输，高地址寄存器后被传输。
                 01 从机地址
                 03 功能码
                 04 字节数
                 12 数据1高字节(0010H)
                 34 数据1低字节(0010H)
                 02 数据2高字节(0011H)
                 03 数据2低字节(0100H)
                 FF CRC高字节
                 F4 CRC低字节

         读一个保持寄存器例子:
                 发送：	01 03 00 10 00 01            85 CF ---- 读 0010H一个寄存器内容
                 返回：	01 03 02 12 34               B5 33 ---- 返回10H功能码写入的内容（10H功能码会介绍）
 */

        uint8_t recAddr = Rx_Buffer[0];                          /* 设备地址 */
        uint16_t recRegAddr = ((Rx_Buffer[2] << 8) | Rx_Buffer[3]); /* 寄存器首地址 */
        uint16_t regNum = ((Rx_Buffer[4] << 8) | Rx_Buffer[5]);     /* 寄存器数量 */
#if (DEBUG_MODBUS != 0)
        elog_debug(" Addr %d, RegAddr %d, regNum %d", recAddr, recRegAddr, regNum);
#endif

        /* 响应 */
        Tx_Buffer[0] = recAddr;         /* 第1个字节 设备地址 */
        Tx_Buffer[1] = Rx_Buffer[1];     /* 第2个字节 功能码 */
        Tx_Buffer[2] = regNum;          /* 第3个字节 寄存器数量 */

        /* 返回保持寄存器内的数据 */
        for (uint8_t i = 0; i < regNum; i++) {
                Tx_Buffer[2 * i + 3] = i + 11;
                Tx_Buffer[2 * i + 4] = i + 22;
#if (DEBUG_MODBUS != 0)
                elog_debug(" R%d: (%d)%d, (%d)%d", i, 2 * i + 3, Tx_Buffer[2 * i + 3], 2 * i + 4, Tx_Buffer[2 * i + 4]);
#endif
        }

        /* 拼接CRC */
        uint16_t crc16 = ModbusCRC16((uint8_t *)&Tx_Buffer[0], 2 * regNum + 4);

        Tx_Buffer[2 * (regNum - 1) + 5] = crc16 >> 8;
        Tx_Buffer[2 * (regNum - 1) + 6] = crc16 & 0xFF;
#if (DEBUG_MODBUS != 0)
        elog_debug(" CRC:%04X  (%d)%02X, (%d)%02X L:%d", crc16, 2 * (regNum - 1) + 5, Tx_Buffer[2 * (regNum - 1) + 5],
                   2 * (regNum - 1) + 6, Tx_Buffer[2 * (regNum - 1) + 6], 2 * (regNum - 1) + 7);
#endif
#if (DEBUG_MODBUS != 0)
        printd("\r\n TxBuffer:");
        for (uint8_t i = 0; i < 2 * (regNum - 1) + 7; i++) {
                printd(" (%d)%02X", i, Tx_Buffer[i]);
        }
        printd("\r\n");
#endif

        /* 只有无报错时才回复 */
        if (modbus.ErrorState == ERR_NOT) {
                mb_Send(2 * (regNum - 1) + 7);
#if (DEBUG_MODBUS != 0)
                XF_LOG_BUFFER_HEX(Tx_Buffer, 2 * (regNum - 1) + 7);
#endif
        }
}

/**
 * @brief     : 06 写单个保持寄存器
 */
void mb_06H(void)
{
        /*
                写保持寄存器。注意06指令只能操作单个保持寄存器，10H指令可以设置单个或多个保持寄存器
                主机发送:
                        01 从机地址
                        06 功能码
                        00 寄存器地址高字节
                        10 寄存器地址低字节
                        67 数据1高字节
                        4A 数据1低字节
                        23 CRC校验高字节
                        C8 CRC校验低字节

                从机响应:
                        01 从机地址
                        06 功能码
                        00 寄存器地址高字节
                        10 寄存器地址低字节
                        67 数据1高字节
                        4A 数据1低字节
                        23 CRC校验高字节
                        C8 CRC校验低字节

                例子:
                        发送：	01 06 00 10 67 4A  23 C8    ---- 将0010地址寄存器设置为67 4A
                        返回：	01 06 00 10 67 4A  23 C8    ---- 返回同样数据
*/
        uint8_t recAddr = Rx_Buffer[0];                             /* 设备地址 */
        uint16_t recRegAddr = ((Rx_Buffer[2] << 8) | Rx_Buffer[3]); /* 寄存器首地址 */
        uint16_t regNum = ((Rx_Buffer[4] << 8) | Rx_Buffer[5]);     /* 寄存器数量 */
#if (DEBUG_MODBUS != 0)
        elog_debug(" Addr %d, RegAddr %d, regNum %d", recAddr, recRegAddr, regNum);
#endif
}

/**
 * @brief     : 10 写多个保持寄存器
 */
void mb_10H(void)
{
        /*
        主机发送:
                01 从机地址
                10 功能码
                00 寄存器起始地址高字节
                10 寄存器起始地址低字节
                00 寄存器数量高字节
                02 寄存器数量低字节
                04 字节数
                12 数据1高字节
                34 数据1低字节
                02 数据2高字节
                03 数据2低字节
                F7 CRC校验高字节
                74 CRC校验低字节

        从机响应:
                01 从机地址
                10 功能码
                00 寄存器地址高字节
                10 寄存器地址低字节
                00 寄存器数量高字节
                02 寄存器数量低字节
                40 CRC校验高字节
                0D CRC校验低字节

        例子:
                发送：	01 10 00 10 00 02 04 12 34 02 03 F7 74    ----  向0010H~0011H写入12 34 02 03 四个字节数据
                返回：	01 10 00 10 00 02 40 0D                   ----  返回内容

*/
        uint8_t recAddr = Rx_Buffer[0];                             /* 设备地址 */
        uint16_t recRegAddr = ((Rx_Buffer[2] << 8) | Rx_Buffer[3]); /* 寄存器首地址 */
        uint16_t regNum = ((Rx_Buffer[4] << 8) | Rx_Buffer[5]);     /* 寄存器数量 */
#if (DEBUG_MODBUS != 0)
        elog_debug(" Addr %d, RegAddr %d, regNum %d", recAddr, recRegAddr, regNum);
#endif
}

void mb_Poll(void)
{
        if (MB_RECIVE_END == modbus.RunState) {
                if (LEAST_RCV_CNT < modbus.ReciveCount) {
                        LED_WORK = !LED_WORK;
                        XF_LOG_BUFFER_HEX(Rx_Buffer, modbus.ReciveCount);
                        if (0 == ModbusCRC16(&Rx_Buffer[0], modbus.ReciveCount)) {
                                /* 第2字节 功能码 */
                                /* 确认模块存在并且工作正常 */
                                switch (Rx_Buffer[1]) {
#if (SUPPORT_FUN_CODE_03H != 0)
                                        case FUN_CODE_03H:
                                                mb_03H();
                                                break;
#endif
#if (SUPPORT_FUN_CODE_06H != 0)
                                        case FUN_CODE_06H:
                                                mb_06H();
                                                break;
#endif
#if (SUPPORT_FUN_CODE_10H != 0)
                                        case FUN_CODE_10H:
                                                mb_10H();
                                                break;
#endif
                                        default:
                                                modbus.ErrorState = ERR_MB_FUN;
                                                break;
                                }
                        } else {
                                modbus.ErrorState = ERR_MB_DEVICE;
                                elog_error("CRC Error");
                        }
                        modbus.ReciveCount = 0;
                        mb_Error();
                        modbus.ErrorState = MB_IDLE;
                } else {
                        modbus.ReciveCount = 0;
                        mb_Error();
                        modbus.ErrorState = MB_IDLE;
                }
        }
}
