#define _AGS_MB_GLOBALS_
#include "common.h"

ELAB_TAG("ags_mb"); /* elog 标签 */

uint16_t BaudRate_V[BAUD_NUM] = {9600, 9600, 19200, 38400};
uint16_t BaudRate_Time[BAUD_NUM] = {520, 520, 260, 130};

void ags_mbInit(void)
{
        unsigned char cnt;
        RX_EN(); /* 开机为接收模式 */

        I2CPageRead_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.baudrate);
        if (BAUD_38400 < syspara.baudrate) {
                syspara.baudrate = BAUD_9600; /* 9600 */
        }

        if (BAUD_19200 == syspara.baudrate) /* 19200 */
        {
                Usart2_Init(36, BAUD_RATE_19200); /* UART2 19200bps */
                delay_ms(100);
                Usart3_Init(36, BAUD_RATE_19200); /* UART2 19200bps */
                delay_ms(100);
                TIM3_Init(MODBUS_TIME_19200, 71);  // 45us--0.45ms
        } else if (BAUD_38400 == syspara.baudrate) /* 38400 */
        {
                Usart2_Init(36, BAUD_RATE_38400); /* UART2 38400bps */
                delay_ms(100);
                Usart3_Init(36, BAUD_RATE_38400); /* UART3 38400bps */
                delay_ms(100);
                TIM3_Init(MODBUS_TIME_38400, 71);
        } else /* Default 9600 */
        {
                syspara.baudrate = BAUD_9600;
                Usart2_Init(36, BAUD_RATE_9600); /* UART2 9600bps */
                delay_ms(100);
                Usart3_Init(36, BAUD_RATE_9600); /* UART3 9600bps */
                delay_ms(100);
                TIM3_Init(MODBUS_TIME_9600, 71);  // 45us--0.45ms
        }
        delay_ms(100);
        elog_debug(" Init AGS UART2/3 Baud:%dbps", BaudRate_V[syspara.baudrate]);

        // 参数配置
        ags_mbParam.sRUN = MB_IDLE;
        ags_mbParam.sERR = ERR_NOT;
        ags_mbParam.times = 0;
        ags_mbParam.rCnt = 0;
        ags_mbParam.HostRept = 0;
        ags_mbParam.mAddrs = MB_SALVE_DEF_ADDR;
        for (cnt = 0; cnt < LENGTH_MB_DATA; cnt++) {
                ags_mbParam.rBuf[cnt] = 0;
                ags_mbParam.tBuf[cnt] = 0;
        }

        for (cnt = 0; cnt < LENGTH_COIL_REG; cnt++)
                CoilBitPara[cnt] = 0;

        for (cnt = 0; cnt < LENGTH_DiscreteREG; cnt++)
                DiscreteBitPara[cnt] = 0;

        for (cnt = 0; cnt < LENGTH_HoldingREG; cnt++)
                HoldingREGPara[cnt] = 0;

        for (cnt = 0; cnt < LENGTH_InputREG; cnt++)
                InputREGPara[cnt] = 0;
}

void ags_mbTimesProcess(void)
{
        if (ags_mbParam.sRUN & MB_BUSY) {
                /* 总线检测 */
                if (ags_mbParam.times < BUS_IDLE_TIME) {
                        ags_mbParam.times++;
                } else {
                        // 总线进入空闲,帧结束或开始
                        if (ags_mbParam.sRUN == MB_RECIVE_ERR) {
                                // 接收过程中 有出现数据存储空间溢出或间隔时间超过T1.5
                                ags_mbParam.sERR = ERR_MB_DEVICE;
                                ags_mbParam.sRUN = MB_IDLE;
                        } else if (ags_mbParam.sRUN == MB_NO_RESPONSE) {
                                ags_mbParam.sRUN = MB_IDLE;
                        } else if (ags_mbParam.sRUN == MB_RECIVE) {
                                ags_mbParam.sRUN = MB_RECIVE_END;
                        }
                }
        }
}

void ags_mbSend(unsigned char length)
{
        unsigned char cnt;

        TX_EN();
        if (length) {
                ags_mbParam.sRUN = MB_SEND;
                for (cnt = 0; cnt < length; cnt++) {
                        while ((USART3->SR & 0X40) == 0)
                                ;  // 等待发送结束
                        USART3->DR = ags_mbParam.tBuf[cnt];
                        ags_mbParam.times = 0;  // 重新计时。
                }
        }
        while ((USART3->SR & 0X40) == 0)
                ;  // 等待发送结束
        RX_EN();

        if (length) {
                ags_mbParam.sRUN = MB_SEND;
                for (cnt = 0; cnt < length; cnt++) {
                        while ((USART2->SR & 0X40) == 0)
                                ;  // 等待发送结束
                        USART2->DR = ags_mbParam.tBuf[cnt];
                        ags_mbParam.times = 0;  // 重新计时。
                }
        }
        while ((USART2->SR & 0X40) == 0)
                ;  // 等待发送结束
        ags_mbParam.sRUN = MB_IDLE;
        ags_mbParam.rCnt = 0;
}

void ags_mbReceive(unsigned char res)
{
        ags_mbParam.times = 0;  // 重新计时
        if (ags_mbParam.sRUN == MB_IDLE && !ags_mbParam.rCnt) {
                // 空闲并且数据处理结束,可以进行新的接收
                if (ags_mbParam.mAddrs == res || res == MB_Broadcast_ADDR) {
                        // 开始接收数据
                        ags_mbParam.sRUN = MB_RECIVE;
                        ags_mbParam.sERR = ERR_NOT;
                        ags_mbParam.rCnt = 1;
                        ags_mbParam.rBuf[0] = res;
                } else {
                        // 非本设备地址,或非当前查询的设备,且非广播地址
                        ags_mbParam.sRUN = MB_NO_RESPONSE;
                        //            ags_mbParam.sERR = ERR_MB_DEVICE_ADDR;
                        ags_mbParam.sERR = ERR_MB_DEVICE;
                        elog_error("%d Address Error", res);
                }
        } else if (ags_mbParam.sRUN == MB_RECIVE) {
                // 如果溢出或者传输过程出现时间间隔超过T1.5,都不在接收
                if (ags_mbParam.rCnt < LENGTH_MB_DATA && ags_mbParam.times < FRAME_ERR_TIME) {
                        ags_mbParam.rBuf[ags_mbParam.rCnt] = res;
                } else {
                        ags_mbParam.sRUN = MB_RECIVE_ERR;
                        elog_error("%d Frame Error", res);
                }
                ++ags_mbParam.rCnt;
        }
}

void ags_mbError(void)
{
        uint16_t temp16;

        if (ERR_MB_FUN == ags_mbParam.sERR || ERR_MB_ADDR == ags_mbParam.sERR || ERR_MB_DATA == ags_mbParam.sERR) {
                /* 从模式,发送响应数据 */
                ags_mbParam.tBuf[0] = ags_mbParam.rBuf[0];         // 设备地址
                ags_mbParam.tBuf[1] = ags_mbParam.rBuf[1] | 0X80;  // 功能码| 0X80
                ags_mbParam.tBuf[2] = ags_mbParam.sERR;            // 错误代码
                temp16 = ModbusCRC16(&ags_mbParam.tBuf[0], 3);     // 获取CRC
                ags_mbParam.tBuf[3] = temp16 >> 8;
                ags_mbParam.tBuf[4] = temp16;
                if (ags_mbParam.tBuf[0] != MB_Broadcast_ADDR) {
                        ags_mbSend(5);
                }
#ifdef DEBUG_AGS_MB
                printd("\r\n [%02x]error reply Func:%02x", ags_mbParam.sERR, ags_mbParam.tBuf[1]);
#endif
        }
        ags_mbParam.sERR = ERR_NOT;
}

/* 功能码3 */
void ags_mbReadHoldingRegisters(void)
{
        unsigned short reg_num;
        unsigned char dvc_addr, op_addr, byteCount;

        dvc_addr = ags_mbParam.rBuf[0]; /* 模块地址 */
        op_addr = ags_mbParam.rBuf[2];  /* 操作码/操作地址 */
        /* 地址判断 */
        if (ags_mbParam.mAddrs == dvc_addr || MB_Broadcast_ADDR == dvc_addr) {
                ags_mbParam.tBuf[0] = ags_mbParam.rBuf[0]; /* 模块地址 */
                ags_mbParam.tBuf[1] = ags_mbParam.rBuf[1]; /* 功能码 */
                ags_mbParam.tBuf[2] = ags_mbParam.rBuf[2]; /* 操作码/操作地址 */
                if (0x00 == op_addr)                       /* 读状态 */
                {
                        ags_mbParam.tBuf[3] = Valve.status;         /* 模块状态 */
                        ags_mbParam.tBuf[4] = Valve.portCur;        /* 当前通道 */
                        ags_mbParam.tBuf[5] = ags_mbParam.mAddrs;   /* 模块地址 */
                        ags_mbParam.tBuf[6] = valveFix.fix.portCnt; /* 模块通道数 */
                        ags_mbParam.tBuf[7] = Valve.fDirCw;         /* CW补偿值 */
                        ags_mbParam.tBuf[8] = Valve.fDirCCw;        /* CCW补偿值 */
                        ags_mbParam.tBuf[9] = Valve.spd;            /* 速度 */
                        byteCount = 10;
                } else if (0x01 == op_addr) /* 读当前通道 */
                {
                        ags_mbParam.tBuf[3] = Valve.portCur;  // 通道编号
                        byteCount = 4;
                } else if (0x02 == op_addr) /* 读地址 */
                {
                        ags_mbParam.tBuf[3] = ags_mbParam.mAddrs; /* 地址 */
                        byteCount = 4;
                } else if (0x03 == op_addr) /* 读版本 */
                {
                        ags_mbParam.tBuf[3] = (SOFT_VER >> 24) & 0xFF;
                        ags_mbParam.tBuf[4] = (SOFT_VER >> 16) & 0xFF;
                        ags_mbParam.tBuf[5] = (SOFT_VER >> 8) & 0xFF;
                        ags_mbParam.tBuf[6] = (SOFT_VER >> 0) & 0xFF; /* 模块版本号 */
                        byteCount = 7;
                } else if (0x07 == op_addr) /* 读波特率 */
                {
                        I2CPageRead_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.baudrate);
                        ags_mbParam.tBuf[3] = syspara.baudrate; /* 波特率 */
                        byteCount = 4;
                } else if (0x08 == op_addr) /* 读序列号 */
                {
                        I2CPageRead_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
                        ags_mbParam.tBuf[3] = Valve.SnCode[0];
                        ags_mbParam.tBuf[4] = Valve.SnCode[1];
                        ags_mbParam.tBuf[5] = Valve.SnCode[2];
                        ags_mbParam.tBuf[6] = Valve.SnCode[3];
                        ags_mbParam.tBuf[7] = Valve.SnCode[4];
                        byteCount = 8;
                } else if (0x09 == op_addr) /* 读速度 */
                {
                        ags_mbParam.tBuf[3] = Valve.spd; /* 速度 */
                        byteCount = 4;
                } else if (0x0A == op_addr) /* 读切换次数 */
                {
                        ags_mbParam.tBuf[3] = ((uint8_t *)&syspara.totalCnt)[3];
                        ags_mbParam.tBuf[4] = ((uint8_t *)&syspara.totalCnt)[2];
                        ags_mbParam.tBuf[5] = ((uint8_t *)&syspara.totalCnt)[1];
                        ags_mbParam.tBuf[6] = ((uint8_t *)&syspara.totalCnt)[0];
                        if (syspara.totalCnt != syspara.totalCntLst) {
                                I2CPageWrite_Nbytes(ADDR_TOTAL_CNT, LEN_TOTAL_CNT, ((uint8_t *)&syspara.totalCnt));
                                syspara.totalCntLst = syspara.totalCnt;
                        }
                        byteCount = 7;
                } else if (0x0C == op_addr) /* 读停留时间 */
                {
                        I2CPageRead_Nbytes(ADDR_PAUSE_TIME, LEN_PAUSE_TIME, (uint8_t *)&syspara.pauseTime);
                        ags_mbParam.tBuf[3] = ((uint8_t *)&syspara.pauseTime)[3];
                        ags_mbParam.tBuf[4] = ((uint8_t *)&syspara.pauseTime)[2];
                        ags_mbParam.tBuf[5] = ((uint8_t *)&syspara.pauseTime)[1];
                        ags_mbParam.tBuf[6] = ((uint8_t *)&syspara.pauseTime)[0];
                        byteCount = 7;
                }
#if 0
        else if (0x0D == op_addr) /* 读切换时间 */
        {
            I2CPageRead_Nbytes(ADDR_PAUSE_TIME, LEN_PAUSE_TIME, (uint8_t *)&syspara.pauseTime);
            ags_mbParam.tBuf[3] = ((uint8_t *)&syspara.pauseTime)[3];
            ags_mbParam.tBuf[4] = ((uint8_t *)&syspara.pauseTime)[2];
            ags_mbParam.tBuf[5] = ((uint8_t *)&syspara.pauseTime)[1];
            ags_mbParam.tBuf[6] = ((uint8_t *)&syspara.pauseTime)[0];
            byteCount = 7;
        }
#endif
                else if (0x0D == op_addr) /* 读半通道 */
                {
                        I2CPageRead_Nbytes(ADDR_HALF_SEAL, LEN_HALF_SEAL, &Valve.bHalfSeal);
                        ags_mbParam.tBuf[3] = Valve.bHalfSeal;
                        byteCount = 4;
                } else if (0x63 == op_addr) /* 读通道数 */
                {
                        I2CPageRead_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
                        ags_mbParam.tBuf[3] = valveFix.fix.portCnt;
                        byteCount = 4;
                } else {
                        ags_mbParam.sERR = ERR_MB_ADDR; /* 非法数据地址 */
                }
                /* 拼接 */
                reg_num = ModbusCRC16(&ags_mbParam.tBuf[0], byteCount); /* 获取CRC */
                ags_mbParam.tBuf[byteCount] = reg_num >> 8;
                byteCount++;
                ags_mbParam.tBuf[byteCount] = reg_num;
                byteCount++;
                /* 只有当地址不为广播地址且无报错时才回复 可以通过广播地址02查地址 */
                if (((MB_Broadcast_ADDR != ags_mbParam.tBuf[0]) ||
                     ((MB_Broadcast_ADDR == ags_mbParam.tBuf[0]) && 0x02 == op_addr)) &&
                    (ERR_NOT == ags_mbParam.sERR)) {
                        ags_mbSend(byteCount); /* 回复 */
                }
#ifdef DEBUG_AGS_MB
                printd("\r s:");
                for (uint8_t i = 0; i < byteCount; i++)
                        printd(" %02x", ags_mbParam.tBuf[i]);
#endif
        } else {
                ags_mbParam.sERR = ERR_MB_DEVICE_ADDR; /* 非法从站设备地址 */
        }
}

/* 功能码6 */
void ags_mbPresetSingleHoldingRegister(void)
{
        uint16_t reg_num;
        uint8_t dvc_addr, op_addr, byteCount;

        dvc_addr = ags_mbParam.rBuf[0]; /* 第1字节 站号 模块地址 */
        op_addr = ags_mbParam.rBuf[2];  /* 第3字节 操作码 操作地址 */
        if ((dvc_addr == ags_mbParam.mAddrs) && BURN_ADDR != dvc_addr) {
                ags_mbParam.tBuf[0] = ags_mbParam.rBuf[0]; /* 设备地址 */
                ags_mbParam.tBuf[1] = ags_mbParam.rBuf[1]; /* 功能码 */
                ags_mbParam.tBuf[2] = ags_mbParam.rBuf[2]; /* 操作码/操作地址 */
                if (0x00 == op_addr)                       /* 写通道A */
                {
                        if ((ags_mbParam.rBuf[3] && valveFix.fix.portCnt >= ags_mbParam.rBuf[3]) &&
                            (6 == ags_mbParam.rCnt)) {
                                if (VALVE_RUN_END == Valve.status) {
                                        Valve.portDes = ags_mbParam.rBuf[3];
                                        Valve.dir = 0xff; /* 就近切换 */
                                        I2CPageRead_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
                                        if (tBoundary.spd_min > Valve.spd || tBoundary.spd_max < Valve.spd)
                                                Valve.spd = tBoundary.spd_min;
                                        speed[AXSV] = accel[AXSV] = 100;
                                        decel[AXSV] = 200;
                                        speed[AXSV] *= (Valve.spd);
                                        speed[AXSV] *= (rdc.rate);
                                        accel[AXSV] *= (Valve.spd);
                                        accel[AXSV] *= (rdc.rate);
                                        decel[AXSV] *= (Valve.spd);
                                        decel[AXSV] *= (rdc.rate);
                                } else if (VALVE_ERR == Valve.status) {
                                        ags_mbParam.sERR = ERR_MB_ERROR; /* 从设备故障 */
                                } else {
                                        ags_mbParam.sERR = ERR_MB_BUSY; /* 从设备忙 */
                                }
                        } else {
                                ags_mbParam.sERR = ERR_MB_DATA; /* 操作数据无效 */
                        }
                } else if (0x01 == op_addr) /* 写地址 */
                {
                        //     if((AGS_ADDR_MIN <= ags_mbParam.rBuf[3] && AGS_ADDR_MAX >= ags_mbParam.rBuf[3]) &&
                        if ((AGS_ADDR_MAX >= ags_mbParam.rBuf[3]) && (6 == ags_mbParam.rCnt)) {
                                ags_mbParam.mAddrs = ags_mbParam.rBuf[3];
                                modbus.Address = ags_mbParam.mAddrs;
                                I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &ags_mbParam.mAddrs);
                        } else {
                                ags_mbParam.sERR = ERR_MB_DATA; /* 操作数据无效 */
                        }
                } else if (0x06 == op_addr) /* 复位*/
                {
                        if (0x00 == ags_mbParam.rBuf[3] && 6 == ags_mbParam.rCnt) {
                                // 复位指令
                                Valve.status = VALVE_INITING;
                                Valve.initStep = 0; /* 复位指令 */
                                Valve.bNewInit = 0xff;
                                Valve.passByOne = 0;
                                Valve.bReInit = 1;
                                Valve.goFirstFlag = 0;
                                Valve.ErrBlinkTime = RETRY_TIME_OUT;
                                I2CPageRead_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
                                (CHANNEL_MIN > valveFix.fix.portCnt || CHANNEL_MAX < valveFix.fix.portCnt)
                                    ? (valveFix.fix.portCnt = CHANNEL_DEF)
                                    : (valveFix.fix.portCnt);
                                I2CPageRead_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
                                I2CPageRead_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
                        } else {
                                ags_mbParam.sERR = ERR_MB_DATA; /* 操作数据无效 */
                        }
                } else if (0x07 == op_addr) /* 写波特率 */
                {
                        if ((BAUD_9600 <= ags_mbParam.rBuf[3] && BAUD_38400 >= ags_mbParam.rBuf[3]) &&
                            6 == ags_mbParam.rCnt) {
                                switch (ags_mbParam.rBuf[3]) {
                                        case BAUD_9600:
                                                syspara.baudrate = BAUD_9600;
                                                break;
                                        case BAUD_19200:
                                                syspara.baudrate = BAUD_19200;
                                                break;
                                        case BAUD_38400:
                                                syspara.baudrate = BAUD_38400;
                                                break;
                                        default:
                                                break;
                                }
                                I2CPageWrite_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.baudrate);
                        } else {
                                ags_mbParam.sERR = ERR_MB_DATA; /* 操作数据无效 */
                        }
                } else if (0x08 == op_addr) /* 写序列号 */
                {
                        if (10 == ags_mbParam.rCnt) {
                                Valve.SnCode[0] = ags_mbParam.rBuf[3];
                                Valve.SnCode[1] = ags_mbParam.rBuf[4];
                                Valve.SnCode[2] = ags_mbParam.rBuf[5];
                                Valve.SnCode[3] = ags_mbParam.rBuf[6];
                                Valve.SnCode[4] = ags_mbParam.rBuf[7];
                                I2CPageWrite_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
                        } else {
                                ags_mbParam.sERR = ERR_MB_DATA; /* 操作数据无效 */
                        }
                } else if (0x09 == op_addr) /* 写速度 */
                {
                        if ((tBoundary.spd_min <= ags_mbParam.rBuf[3] && tBoundary.spd_max >= ags_mbParam.rBuf[3]) &&
                            6 == ags_mbParam.rCnt) {
                                Valve.spd = ags_mbParam.rBuf[3];
                                I2CPageWrite_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
                        } else if (0 == ags_mbParam.rBuf[3]) {
                                Valve.spd = tBoundary.spd_init;
                                I2CPageWrite_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
                        } else {
                                ags_mbParam.sERR = ERR_MB_DATA; /* 操作数据无效 */
                        }
                } else if (0x0A == op_addr) /* 写切换次数 */
                {
                        if (9 == ags_mbParam.rCnt) {
                                ((uint8_t *)&syspara.totalCnt)[0] = ags_mbParam.rBuf[6];
                                ((uint8_t *)&syspara.totalCnt)[1] = ags_mbParam.rBuf[5];
                                ((uint8_t *)&syspara.totalCnt)[2] = ags_mbParam.rBuf[4];
                                ((uint8_t *)&syspara.totalCnt)[3] = ags_mbParam.rBuf[3];
                                I2CPageWrite_Nbytes(ADDR_TOTAL_CNT, LEN_TOTAL_CNT, (uint8_t *)&syspara.totalCnt);
                        } else {
                                ags_mbParam.sERR = ERR_MB_DATA; /* 操作数据无效 */
                        }
                } else if (0x0C == op_addr) /* 写停留时间 */
                {
                        ((uint8_t *)&syspara.pauseTime)[0] = ags_mbParam.rBuf[6];
                        ((uint8_t *)&syspara.pauseTime)[1] = ags_mbParam.rBuf[5];
                        ((uint8_t *)&syspara.pauseTime)[2] = ags_mbParam.rBuf[4];
                        ((uint8_t *)&syspara.pauseTime)[3] = ags_mbParam.rBuf[3];
                        if (9 == ags_mbParam.rCnt && syspara.pauseTime <= ADZ_PT_MAX) {
                                I2CPageWrite_Nbytes(ADDR_PAUSE_TIME, LEN_PAUSE_TIME, (uint8_t *)&syspara.pauseTime);
                        } else {
                                syspara.pauseTime = 0;
                                ags_mbParam.sERR = ERR_MB_DATA; /*  */
                        }
                } else if (0x0D == op_addr) /* 写半通道 */
                {
                        if ((OFF == ags_mbParam.rBuf[3] || ON == ags_mbParam.rBuf[3]) && 6 == ags_mbParam.rCnt) {
                                Valve.bHalfSeal = ags_mbParam.rBuf[3];
                                I2CPageWrite_Nbytes(ADDR_HALF_SEAL, LEN_HALF_SEAL, &Valve.bHalfSeal);
                        } else {
                                ags_mbParam.sERR = ERR_MB_DATA; /* 操作数据无效 */
                        }
                }
                //        else if(0xFF == op_addr)
                //        {
                //            printd("\r\n rcv para: %02x %02x %02x %02x",
                //                   ags_mbParam.rBuf[3], ags_mbParam.rBuf[5], ags_mbParam.rBuf[6],
                //                   ags_mbParam.rBuf[7]);
                //            I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &ags_mbParam.rBuf[3]);
                //            I2CPageWrite_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &ags_mbParam.rBuf[6]);
                //            I2CPageWrite_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &ags_mbParam.rBuf[7]);
                //            I2CPageWrite_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &ags_mbParam.rBuf[5]);
                //        }
                else {
                        ags_mbParam.sERR = ERR_MB_ADDR; /* 超出操作地址范围或者操作地址无效 */
                }
                ags_mbParam.tBuf[3] = ags_mbParam.rBuf[3]; /* 响应报文此位不变表示该指令应答成功 */
                byteCount = 4;
                reg_num = ModbusCRC16(&ags_mbParam.tBuf[0], byteCount); /* 获取CRC */
                ags_mbParam.tBuf[byteCount] = reg_num >> 8;
                byteCount++;
                ags_mbParam.tBuf[byteCount] = reg_num;
                byteCount++;
                /* 只有当地址不为广播地址且无报错时才回复 */
                if ((ags_mbParam.tBuf[0] != MB_Broadcast_ADDR) && (ERR_NOT == ags_mbParam.sERR)) {
                        ags_mbSend(byteCount); /* 回复 */
                }
#ifdef DEBUG_AGS_MB
                printd("\r s:");
                for (uint8_t i = 0; i < byteCount; i++)
                        printd(" %02x", ags_mbParam.tBuf[i]);
#endif
        } else {
                ags_mbParam.sERR = ERR_MB_DEVICE_ADDR; /* 非法从站设备地址 */
        }
}

/* 功能码 0x10 */
void ags_mbPresetMultipleHoldingRegisters(void)
{
        unsigned short reg_num;
        unsigned char dvc_addr, op_addr, byteCount;

        dvc_addr = ags_mbParam.rBuf[0]; /* 模块地址 */
        op_addr = ags_mbParam.rBuf[2];  /* 端口编号 */
        if (dvc_addr <= AGS_ADDR_MAX) {
                ags_mbParam.tBuf[0] = ags_mbParam.rBuf[0]; /* 设备地址 */
                ags_mbParam.tBuf[1] = ags_mbParam.rBuf[1]; /* 功能码 */
                ags_mbParam.tBuf[2] = ags_mbParam.rBuf[2]; /* 操作码/操作地址 */

                /* 地址  10功能码  00操作码  通道  速度  方向  CRC*2 */
                if (0x00 == op_addr) {
                        if ((ags_mbParam.rBuf[3] && valveFix.fix.portCnt >= ags_mbParam.rBuf[3]) &&
                            ((tBoundary.spd_min <= ags_mbParam.rBuf[4] && tBoundary.spd_max >= ags_mbParam.rBuf[4]) ||
                             0 == ags_mbParam.rBuf[4]) &&
                            (VALVE_DIR_CW == ags_mbParam.rBuf[5] || VALVE_DIR_CCW == ags_mbParam.rBuf[5] ||
                             VALVE_DIR_NER == ags_mbParam.rBuf[5]) &&
                            8 == ags_mbParam.rCnt) {
                                if (Valve.status == VALVE_RUN_END) {
                                        Valve.portDes = ags_mbParam.rBuf[3]; /* 目标通道 */
                                } else if (Valve.status == VALVE_RUNNING)    /* 正在运行可记录4条连续指令 */
                                {
                                        if (Valve.serialNum < PREPORTCNT) {
                                                Valve.serialPort[Valve.serialNum++] = ags_mbParam.rBuf[3];
                                        }
                                        ags_mbParam.sERR = ERR_MB_AFFIRM; /* 从设备确认 */
                                } else if (VALVE_ERR == Valve.status) {
                                        ags_mbParam.sERR = ERR_MB_ERROR; /* 从设备故障 */
                                } else {
                                        ags_mbParam.sERR = ERR_MB_BUSY; /* 从设备忙 */
                                }
                                uint8_t tempSpd = 0;
                                if (0 == ags_mbParam.rBuf[4]) {
                                        /* 小于最小速度均按最小速度处理 */
                                        tempSpd = tBoundary.spd_min;
                                } else {
                                        /* 临时速度 */
#ifdef LIMIT_TEMP_SPD
                                        /* 限制临时速度 仅D版特殊定制使用! */
                                        if (ags_mbParam.rBuf[4] <= 30) {
                                                tempSpd = ags_mbParam.rBuf[4];
                                        } else {
                                                /* 临时速度超过30部分限制 */
                                                tempSpd = 30 + (ags_mbParam.rBuf[4] - 30) / 2;
                                        }
                                        printd("\r\n (LTS) Input:%d =Down=> tempSpd:%d ", ags_mbParam.rBuf[4], tempSpd);
#else
                                        tempSpd = ags_mbParam.rBuf[4];
                                        printd("\r\n tempSpd:%d ", tempSpd);
#endif
                                }
                                Valve.dir = ags_mbParam.rBuf[5]; /* 方向 */
                                speed[AXSV] = accel[AXSV] = 100;
                                decel[AXSV] = 200;
                                speed[AXSV] *= (tempSpd);
                                speed[AXSV] *= (rdc.rate);
                                accel[AXSV] *= (tempSpd);
                                accel[AXSV] *= (rdc.rate);
                                decel[AXSV] *= (tempSpd);
                                decel[AXSV] *= (rdc.rate);
                        } else {
                                ags_mbParam.sERR = ERR_MB_DATA; /* 操作数据无效 */
                        }
                        /* 响应报文此位不变表示该指令应答成功 */
                        ags_mbParam.tBuf[3] = ags_mbParam.rBuf[3];
                        byteCount = 4;
                        reg_num = ModbusCRC16(&ags_mbParam.tBuf[0], byteCount); /* 获取CRC */
                        ags_mbParam.tBuf[byteCount] = reg_num >> 8;
                        byteCount++;
                        ags_mbParam.tBuf[byteCount] = reg_num;
                        byteCount++;
                        /* 只有当地址不为广播地址且无报错时才回复 */
                        if ((ags_mbParam.tBuf[0] != MB_Broadcast_ADDR) && (ERR_NOT == ags_mbParam.sERR)) {
                                ags_mbSend(byteCount); /* 回复 */
                        }
#ifdef DEBUG_AGS_MB
                        printd("\r s:");
                        for (uint8_t i = 0; i < byteCount; i++)
                                printd(" %02x", ags_mbParam.tBuf[i]);
#endif
                } else {
                        ags_mbParam.sERR = ERR_MB_ADDR; /* 操作地址无效 */
                }
        } else {
                /* 非法从站设备地址 */
                ags_mbParam.sERR = ERR_MB_DEVICE_ADDR;
        }
}

void ags_mbProcess(void)
{
        if (MB_RECIVE_END == ags_mbParam.sRUN) {
                if (LEAST_RCV_CNT < ags_mbParam.rCnt) {
                        LED_WORK = !LED_WORK;
#ifdef DEBUG_AGS_MB
                        XF_LOG_BUFFER_HEX(ags_mbParam.rBuf, ags_mbParam.rCnt);
#endif
                        if (0 == ModbusCRC16(&ags_mbParam.rBuf[0], ags_mbParam.rCnt)) {
#ifdef DEBUG_AGS_MB
                                printd("\r r:");
                                for (uint8_t i = 0; i < ags_mbParam.rCnt; i++)
                                        printd(" %02x", ags_mbParam.rBuf[i]);
#endif
                                /* 第2字节 功能码 */
                                /* 确认模块存在并且工作正常 */
                                switch (ags_mbParam.rBuf[1]) {
                                        /* 03 读功能码 */
                                        case GET_HOLDING_REGT:
                                                /* 读指令长度不匹配 重新接收 */
                                                if (ags_mbParam.rCnt != 5) {
                                                        ags_mbParam.rCnt = 0;
                                                        ags_mbParam.sRUN = MB_IDLE;
                                                        return;
                                                }
                                                ags_mbReadHoldingRegisters();
                                                break;
                                        /* 06 写功能码 */
                                        case PRESET_HOLDING_sREGT:
                                                ags_mbPresetSingleHoldingRegister();
                                                break;
                                        /* 10 复合写功能码 */
                                        case PRESET_HOLDING_mREGT:
                                                ags_mbPresetMultipleHoldingRegisters();
                                                break;
                                        /* 功能码错误/不支持 */
                                        default:
                                                ags_mbParam.sERR = ERR_MB_FUN;
                                                break;
                                }
                        } else {
                                /* CRC校验失败 */
                                ags_mbParam.sERR = ERR_MB_DEVICE;
                        }
                        ags_mbParam.rCnt = 0;
                        ags_mbError();
                        ags_mbParam.sRUN = MB_IDLE;
                } else {
                        /* 数据长度不足 即无效数据 */
                        ags_mbParam.rCnt = 0;
                        ags_mbError();
                        ags_mbParam.sRUN = MB_IDLE;
                }
        }
}
