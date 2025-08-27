#define _MODBUS_GLOBALS_
#include "common.h"


void ModbusInit(void)
{
    unsigned char cnt;
    RX_EN();        /* 开机为接收模式 */

    I2CPageRead_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.bdrate);
    if(UART_BAUD_38400 < syspara.bdrate)
    {
        syspara.bdrate = BAUD_RATE_19200;  /* 19200 */
    }

    if(UART_BAUD_9600 == syspara.bdrate)         /* 9600 */
    {
        Usart2_Init(36, BAUD_RATE_9600);   /* UART2 9600bps */
        delay_ms(100);
        Usart3_Init(36, BAUD_RATE_9600);   /* UART3 9600bps */
        delay_ms(100);
        TIM3_Init(MODBUS_TIME_9600, 71);   // 45us--0.45ms
    }
    else if(UART_BAUD_38400 == syspara.bdrate)   /* 38400 */
    {
        Usart2_Init(36, BAUD_RATE_38400);   /* UART2 38400bps */
        delay_ms(100);
        Usart3_Init(36, BAUD_RATE_38400);   /* UART3 38400bps */
        delay_ms(100);
        TIM3_Init(MODBUS_TIME_38400, 71);
    }
    else                                            /* Default 19200 */
    {
        Usart2_Init(36, BAUD_RATE_19200);   /* UART2 19200bps */
        delay_ms(100);
        Usart3_Init(36, BAUD_RATE_19200);   /* UART2 19200bps */
        delay_ms(100);
        TIM3_Init(MODBUS_TIME_19200, 71);   // 45us--0.45ms
    }
    delay_ms(100);
    printd("\r Init AGS UART2/3 Baud:%d", syspara.bdrate);

//	Usart2_Init(36, BAUD_RATE_MODBUS);	        // 串口2 232初始化为115200
//	Usart3_Init(36, BAUD_RATE_MODBUS);	        // 串口2 485初始化为115200
//	TIM3_Init(MODBUS_TIME,71);	                // 45us--0.45ms

    // 参数配置
    ModbusPara.sRUN =  MB_IDEL;
    ModbusPara.sERR = ERR_NOT;
    ModbusPara.times = 0;
    ModbusPara.rCnt = 0;
    ModbusPara.HostRept = 0;
    ModbusPara.mAddrs = MB_SALVE_DEF_ADDR;
    for(cnt = 0; cnt < LENGTH_MB_DATA; cnt++)
    {
        ModbusPara.rBuf[cnt] = 0;
        ModbusPara.tBuf[cnt] = 0;
    }

    for(cnt = 0 ; cnt <LENGTH_COIL_REG ; cnt++)
        CoilBitPara[cnt] = 0;

    for(cnt = 0 ; cnt <LENGTH_DiscreteREG ; cnt++)
        DiscreteBitPara[cnt] = 0;

    for(cnt = 0 ; cnt <LENGTH_HoldingREG ; cnt++)
        HoldingREGPara[cnt] = 0;

    for(cnt = 0 ; cnt <LENGTH_InputREG ; cnt++)
        InputREGPara[cnt] = 0;
}


void ModbusTimesProcess(void)
{
    if(ModbusPara.sRUN&MB_BUSY)
    {
        /* 总线检测 */
        if(ModbusPara.times < BUS_IDLE_TIME)
        {
            ModbusPara.times++;
        }
        else
        {
            //总线进入空闲,帧结束或开始
            if(ModbusPara.sRUN == MB_RECIVE_ERR)
            {
                // 接收过程中 有出现数据存储空间溢出或间隔时间超过T1.5
                ModbusPara.sERR = ERR_MB_DEVICE;
                ModbusPara.sRUN =  MB_IDEL;
            }
            else if(ModbusPara.sRUN == MB_NO_RESPONSE)
            {
                ModbusPara.sRUN = MB_IDEL;
            }
            else if(ModbusPara.sRUN == MB_RECIVE)
            {
                ModbusPara.sRUN =  MB_RECIVE_END;
            }
        }
    }
}


void ModbusSend(unsigned char length)
{
    unsigned char cnt;

    TX_EN();
    if(length)
    {
        ModbusPara.sRUN = MB_SEND;
        for(cnt=0; cnt < length; cnt++)
        {
            while((USART3->SR&0X40)==0);        //等待发送结束
            USART3->DR = ModbusPara.tBuf[cnt];
            ModbusPara.times = 0;               //重新计时。
        }
    }
    while((USART3->SR&0X40)==0);                //等待发送结束
    RX_EN();

    if(length)
    {
        ModbusPara.sRUN = MB_SEND;
        for(cnt=0; cnt < length; cnt++)
        {
            while((USART2->SR&0X40)==0);        //等待发送结束
            USART2->DR = ModbusPara.tBuf[cnt];
            ModbusPara.times = 0;               //重新计时。
        }
    }
    while((USART2->SR&0X40)==0);                //等待发送结束
    ModbusPara.sRUN = MB_IDEL;
    ModbusPara.rCnt = 0;
}

void ModbusReceive(unsigned char res)
{
    ModbusPara.times = 0;  //重新计时
    if(ModbusPara.sRUN==MB_IDEL && !ModbusPara.rCnt)
    {
        // 空闲并且数据处理结束,可以进行新的接收
        if(ModbusPara.mAddrs==res || res==MB_Broadcast_ADDR)
        {
            // 开始接收数据
            ModbusPara.sRUN = MB_RECIVE;
            ModbusPara.sERR = ERR_NOT;
            ModbusPara.rCnt = 1;
            ModbusPara.rBuf[0] = res;
        }
        else
        {
            // 非本设备地址,或非当前查询的设备,且非广播地址
            ModbusPara.sRUN = MB_NO_RESPONSE;
//            ModbusPara.sERR = ERR_MB_DEVICE_ADDR;
            ModbusPara.sERR = ERR_MB_DEVICE;
        }
    }
    else if(ModbusPara.sRUN==MB_RECIVE)
    {
        // 如果溢出或者传输过程出现时间间隔超过T1.5,都不在接收
        if(ModbusPara.rCnt < LENGTH_MB_DATA && ModbusPara.times < FRAME_ERR_TIME)
        {
            ModbusPara.rBuf[ModbusPara.rCnt] = res;
        }
        else
        {
            ModbusPara.sRUN = MB_RECIVE_ERR;
        }

        ++ModbusPara.rCnt;
    }
}

void Modbus_ERROR(void)
{
    uint16_t temp16;

    if (ERR_MB_FUN == ModbusPara.sERR || ERR_MB_ADDR == ModbusPara.sERR || 
        ERR_MB_DATA == ModbusPara.sERR)
    {
        /* 从模式,发送响应数据 */
        ModbusPara.tBuf[0] = ModbusPara.rBuf[0];			// 设备地址
        ModbusPara.tBuf[1] = ModbusPara.rBuf[1]|0X80;	    // 功能码| 0X80
        ModbusPara.tBuf[2] = ModbusPara.sERR;			    // 错误代码
        temp16 = ModbusCRC16(&ModbusPara.tBuf[0], 3);	    // 获取CRC
        ModbusPara.tBuf[3] = temp16 >> 8;
        ModbusPara.tBuf[4] = temp16 ;
        if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)
        {
            ModbusSend(5);
        }
#ifdef DEBUG_MODBUS
        printd("\r\n [%02x]error reply Func:%02x",
               ModbusPara.sERR, ModbusPara.tBuf[1]);
#endif
    }
    ModbusPara.sERR = ERR_NOT;
}

/* 功能码3 */
void MB_ReadHoldingRegisters(void)
{
    unsigned short reg_num;
    unsigned char dvc_addr, op_addr, byteCount;

    dvc_addr = ModbusPara.rBuf[0];      /* 模块地址 */
    op_addr = ModbusPara.rBuf[2];       /* 操作码/操作地址 */
    /* 地址判断 */
    if(ModbusPara.mAddrs == dvc_addr || MB_Broadcast_ADDR == dvc_addr)
    {
        ModbusPara.tBuf[0] = ModbusPara.rBuf[0];    /* 模块地址 */
        ModbusPara.tBuf[1] = ModbusPara.rBuf[1];    /* 功能码 */
        ModbusPara.tBuf[2] = ModbusPara.rBuf[2];    /* 操作码/操作地址 */
        if(0x00 == op_addr)                /* 读状态 */
        {
            ModbusPara.tBuf[3] = Valve.status;              /* 模块状态 */
            ModbusPara.tBuf[4] = Valve.portCur;             /* 当前通道 */
            ModbusPara.tBuf[5] = ModbusPara.mAddrs;         /* 模块地址 */
            ModbusPara.tBuf[6] = valveFix.fix.portCnt;      /* 模块通道数 */
            ModbusPara.tBuf[7] = Valve.fDirCw;              /* CW补偿值 */
            ModbusPara.tBuf[8] = Valve.fDirCCw;             /* CCW补偿值 */
            ModbusPara.tBuf[9] = Valve.spd;                 /* 速度 */
            byteCount = 10;
        }
        else if(0x01 == op_addr)           /* 读当前通道 */
        {
            ModbusPara.tBuf[3] = Valve.portCur;             // 通道编号
            byteCount = 4;
        }
        else if(0x02 == op_addr)           /* 读地址 */
        {
            ModbusPara.tBuf[3] = ModbusPara.mAddrs; /* 地址 */
            byteCount = 4;
        }
        else if(0x03 == op_addr)            /* 读版本 */
        {
            ModbusPara.tBuf[3] = (SOFT_VER >> 24) & 0xFF;
            ModbusPara.tBuf[4] = (SOFT_VER >> 16) & 0xFF;
            ModbusPara.tBuf[5] = (SOFT_VER >> 8) & 0xFF;
            ModbusPara.tBuf[6] = (SOFT_VER >> 0) & 0xFF;   /* 模块版本号 */
            byteCount = 7;
        }
        else if(0x07 == op_addr)            /* 读波特率 */
        {
            I2CPageRead_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.bdrate);
            ModbusPara.tBuf[3] = syspara.bdrate;     /* 波特率 */
            byteCount = 4;
        }
        else if(0x08 == op_addr)           /* 读序列号 */
        {
            I2CPageRead_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
            ModbusPara.tBuf[3] = Valve.SnCode[0];
            ModbusPara.tBuf[4] = Valve.SnCode[1];
            ModbusPara.tBuf[5] = Valve.SnCode[2];
            ModbusPara.tBuf[6] = Valve.SnCode[3];
            ModbusPara.tBuf[7] = Valve.SnCode[4];
            byteCount = 8;
        }
        else if(0x09 == op_addr)           /* 读速度 */
        {
            ModbusPara.tBuf[3] = Valve.spd; /* 速度 */
            byteCount = 4;
        }
//        else if(0x0A == op_addr)           /* 读切换次数 */
//        {
//            ModbusPara.tBuf[3] = ((uint8*)&syspara.totalCnt)[3];
//            ModbusPara.tBuf[4] = ((uint8*)&syspara.totalCnt)[2];
//            ModbusPara.tBuf[5] = ((uint8*)&syspara.totalCnt)[1];
//            ModbusPara.tBuf[6] = ((uint8*)&syspara.totalCnt)[0];
//            byteCount = 7;
//        }
        else if(0x63 == op_addr)        /* 读通道数 */
        {
            I2CPageRead_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
            ModbusPara.tBuf[3] = valveFix.fix.portCnt;
            byteCount = 4;
        }
        else
        {
            ModbusPara.sERR = ERR_MB_ADDR;  /* 非法数据地址 */
        }
        /* 拼接 */
        reg_num = ModbusCRC16(&ModbusPara.tBuf[0], byteCount);  /* 获取CRC */
        ModbusPara.tBuf[byteCount] = reg_num >> 8;
        byteCount++;
        ModbusPara.tBuf[byteCount] = reg_num ;
        byteCount++;
        /* 只有当地址不为广播地址且无报错时才回复 可以通过广播地址02查地址 */
        if(((MB_Broadcast_ADDR != ModbusPara.tBuf[0]) ||
                ((MB_Broadcast_ADDR == ModbusPara.tBuf[0]) && 0x02 == op_addr)) && 
                (ERR_NOT == ModbusPara.sERR))
        {
            ModbusSend(byteCount);   /* 回复 */
        }
#ifdef DEBUG_MODBUS
        printd("\r s:");
        for(uint8 i = 0; i < byteCount; i++)
            printd(" %02x", ModbusPara.tBuf[i]);
#endif
    }
    else
    {
        ModbusPara.sERR = ERR_MB_DEVICE_ADDR;   /* 非法从站设备地址 */
    }
}

/* 功能码6 */
void MB_PresetSingleHoldingRegister(void)
{
    uint16_t reg_num;
    uint8_t dvc_addr, op_addr, byteCount;

    dvc_addr = ModbusPara.rBuf[0];  /* 第1字节 站号 模块地址 */
    op_addr = ModbusPara.rBuf[2];   /* 第3字节 操作码 操作地址 */
    if((dvc_addr == ModbusPara.mAddrs) && BURN_ADDR != dvc_addr)
    {
        ModbusPara.tBuf[0] = ModbusPara.rBuf[0];    /* 设备地址 */
        ModbusPara.tBuf[1] = ModbusPara.rBuf[1];    /* 功能码 */
        ModbusPara.tBuf[2] = ModbusPara.rBuf[2];    /* 操作码/操作地址 */
        if(0x00 == op_addr)             /* 写通道A */
        {
            if ((ModbusPara.rBuf[3] && valveFix.fix.portCnt >= ModbusPara.rBuf[3]) &&
                    (6 == ModbusPara.rCnt))
            {
                if(VALVE_RUN_END == Valve.status)
                {
                    Valve.portDes = ModbusPara.rBuf[3];
                    Valve.dir = 0xff;       /* 就近切换 */
                    I2CPageRead_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
                    if(tBoundary.spd_min > Valve.spd || tBoundary.spd_max < Valve.spd)
                        Valve.spd = tBoundary.spd_min;
                    speed[AXSV] = accel[AXSV] = 100;
                    decel[AXSV] = 200;
                    speed[AXSV] *= (Valve.spd);
                    speed[AXSV] *= (rdc.rate);
                    accel[AXSV] *= (Valve.spd);
                    accel[AXSV] *= (rdc.rate);
                    decel[AXSV] *= (Valve.spd);
                    decel[AXSV] *= (rdc.rate);
                }
                else if(VALVE_ERR == Valve.status)
                {
                    ModbusPara.sERR = ERR_MB_ERROR;  /* 从设备故障 */
                }
                else
                {
                    ModbusPara.sERR = ERR_MB_BUSY;  /* 从设备忙 */
                }
            }
            else
            {
                ModbusPara.sERR = ERR_MB_DATA;  /* 操作数据无效 */
            }
        }
        else if(0x01 == op_addr)        /* 写地址 */
        {
            if((AGS_ADDR_MIN <= ModbusPara.rBuf[3] && AGS_ADDR_MAX >= ModbusPara.rBuf[3]) && 
                (6 == ModbusPara.rCnt))
            {
                ModbusPara.mAddrs = ModbusPara.rBuf[3];
                I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &ModbusPara.mAddrs);
            }
            else
            {
                ModbusPara.sERR = ERR_MB_DATA;  /* 操作数据无效 */
            }
        }
        else if(0x06 == op_addr)        /* 复位*/
        {
            if (0x00 == ModbusPara.rBuf[3] && 6 == ModbusPara.rCnt)
            {
                // 复位指令
                Valve.status = VALVE_INITING;
                Valve.initStep = 0;     /* 复位指令 */
                Valve.bNewInit = 0xff;
                Valve.passByOne = 0;
                Valve.bReInit = 1;
                Valve.ErrBlinkTime = RETRY_TIME_OUT;
                I2CPageRead_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
                (CHANNEL_MIN > valveFix.fix.portCnt ||
                 CHANNEL_MAX < valveFix.fix.portCnt) ?
                (valveFix.fix.portCnt = CHANNEL_DEF) :
                (valveFix.fix.portCnt);
                I2CPageRead_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
                I2CPageRead_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
            }
            else
            {
                ModbusPara.sERR = ERR_MB_DATA;  /* 操作数据无效 */
            }
        }
        else if(0x07 == op_addr)        /* 写波特率 */
        {
            if((UART_BAUD_9600 <= ModbusPara.rBuf[3] &&
                    UART_BAUD_38400 >= ModbusPara.rBuf[3]) &&
                    6 == ModbusPara.rCnt)
            {
                syspara.bdrate = ModbusPara.rBuf[3];
                I2CPageWrite_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.bdrate);
            }
            else
            {
                ModbusPara.sERR = ERR_MB_DATA;  /* 操作数据无效 */
            }
        }
        else if(0x08 == op_addr)        /* 写序列号 */
        {
            if (10 == ModbusPara.rCnt)
            {
                Valve.SnCode[0] = ModbusPara.rBuf[3];
                Valve.SnCode[1] = ModbusPara.rBuf[4];
                Valve.SnCode[2] = ModbusPara.rBuf[5];
                Valve.SnCode[3] = ModbusPara.rBuf[6];
                Valve.SnCode[4] = ModbusPara.rBuf[7];
                I2CPageWrite_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
            }
            else
            {
                ModbusPara.sERR = ERR_MB_DATA;   /* 操作数据无效 */
            }
        }
        else if(0x09 == op_addr)        /* 写速度 */
        {
            if((tBoundary.spd_min <= ModbusPara.rBuf[3] && tBoundary.spd_max >= ModbusPara.rBuf[3]) && 
                6 == ModbusPara.rCnt)
            {
                Valve.spd = ModbusPara.rBuf[3];
                I2CPageWrite_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
            }
            else if(0 == ModbusPara.rBuf[3])
            {
                Valve.spd = tBoundary.spd_min;
                I2CPageWrite_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
            }
            else
            { 
                ModbusPara.sERR = ERR_MB_DATA;  /* 操作数据无效 */
            }
        }
//        else if(0xFF == op_addr)
//        {
//            printd("\r\n rcv para: %02x %02x %02x %02x",
//                   ModbusPara.rBuf[3], ModbusPara.rBuf[5], ModbusPara.rBuf[6], ModbusPara.rBuf[7]);
//            I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &ModbusPara.rBuf[3]);
//            I2CPageWrite_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &ModbusPara.rBuf[6]);
//            I2CPageWrite_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &ModbusPara.rBuf[7]);
//            I2CPageWrite_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &ModbusPara.rBuf[5]);
//        }
        else
        {
            ModbusPara.sERR = ERR_MB_ADDR;  /* 超出操作地址范围或者操作地址无效 */
        }
        ModbusPara.tBuf[3] = ModbusPara.rBuf[3];    /* 响应报文此位不变表示该指令应答成功 */
        byteCount = 4;
        reg_num = ModbusCRC16(&ModbusPara.tBuf[0], byteCount); /* 获取CRC */
        ModbusPara.tBuf[byteCount] = reg_num >> 8;
        byteCount++;
        ModbusPara.tBuf[byteCount] = reg_num ;
        byteCount++;
        /* 只有当地址不为广播地址且无报错时才回复 */
        if((ModbusPara.tBuf[0] != MB_Broadcast_ADDR) &&
                (ERR_NOT == ModbusPara.sERR))
        {
            ModbusSend(byteCount);   /* 回复 */
        }
#ifdef DEBUG_MODBUS
        printd("\r s:");
        for(uint8 i=0; i<byteCount; i++)
            printd(" %02x", ModbusPara.tBuf[i]);
#endif
    }
    else
    {
        ModbusPara.sERR = ERR_MB_DEVICE_ADDR;   /* 非法从站设备地址 */
    }
}

/* 功能码 0x10 */
void MB_PresetMultipleHoldingRegisters(void)
{
    unsigned short reg_num;
    unsigned char dvc_addr, op_addr, byteCount;

    dvc_addr = ModbusPara.rBuf[0];      /* 模块地址 */
    op_addr = ModbusPara.rBuf[2];       /* 端口编号 */
    if(dvc_addr <= AGS_ADDR_MAX)
    {
        ModbusPara.tBuf[0] = ModbusPara.rBuf[0];    /* 设备地址 */
        ModbusPara.tBuf[1] = ModbusPara.rBuf[1];    /* 功能码 */
        ModbusPara.tBuf[2] = ModbusPara.rBuf[2];    /* 操作码/操作地址 */

        /* 地址  10功能码  00操作码  通道  速度  方向  CRC*2 */
        if(0x00 == op_addr)
        {
            if ((ModbusPara.rBuf[3] && valveFix.fix.portCnt >= ModbusPara.rBuf[3]) && 
                ((tBoundary.spd_min <= ModbusPara.rBuf[4] && tBoundary.spd_max >= ModbusPara.rBuf[4]) || 
                0 == ModbusPara.rBuf[4]) && 
                (VALVE_DIR_CW == ModbusPara.rBuf[5] || VALVE_DIR_CCW == ModbusPara.rBuf[5] ||
                 VALVE_DIR_NER == ModbusPara.rBuf[5]) && 8 == ModbusPara.rCnt)
            {
                if(Valve.status == VALVE_RUN_END)
                {
                    Valve.portDes = ModbusPara.rBuf[3]; /* 目标通道 */
                }
                else if(Valve.status == VALVE_RUNNING) /* 正在运行可记录4条连续指令 */
                {
                    if(Valve.serialNum < PREPORTCNT)
                    {
                        Valve.serialPort[Valve.serialNum++] = ModbusPara.rBuf[3];
                    }
                    ModbusPara.sERR = ERR_MB_AFFIRM;  /* 从设备确认 */
                }
                else if(VALVE_ERR == Valve.status)
                {
                    ModbusPara.sERR = ERR_MB_ERROR;  /* 从设备故障 */
                }
                else
                {
                    ModbusPara.sERR = ERR_MB_BUSY;  /* 从设备忙 */
                }
                uint8_t tempSpd = 0;
                if(0 == ModbusPara.rBuf[4])
                {
                    /* 小于最小速度均按最小速度处理 */
                    tempSpd = tBoundary.spd_min;
                }
                else
                {
                    /* 临时速度 */
#ifdef LIMIT_TEMP_SPD
                    /* 限制临时速度 仅D版特殊定制使用! */
                    if(30 <= ModbusPara.rBuf[4])
                    {
                        tempSpd = ModbusPara.rBuf[4];
                    }
                    else
                    {
                        /* 临时速度超过30部分限制 */
                        tempSpd = 30 + (ModbusPara.rBuf[4] - 30) / 2;
                    }
#else
                    tempSpd = ModbusPara.rBuf[4];
#endif
                }
                Valve.dir = ModbusPara.rBuf[5];     /* 方向 */
                speed[AXSV] = accel[AXSV] = 100;
                decel[AXSV] = 200;
                speed[AXSV] *= (tempSpd);
                speed[AXSV] *= (rdc.rate);
                accel[AXSV] *= (tempSpd);
                accel[AXSV] *= (rdc.rate);
                decel[AXSV] *= (tempSpd);
                decel[AXSV] *= (rdc.rate);
            }
            else
            {
                ModbusPara.sERR = ERR_MB_DATA;  /* 操作数据无效 */
            }
            /* 响应报文此位不变表示该指令应答成功 */
            ModbusPara.tBuf[3] = ModbusPara.rBuf[3];
            byteCount = 4;
            reg_num = ModbusCRC16( &ModbusPara.tBuf[0], byteCount); /* 获取CRC */
            ModbusPara.tBuf[byteCount] = reg_num >> 8;
            byteCount++;
            ModbusPara.tBuf[byteCount] = reg_num ;
            byteCount++;
            /* 只有当地址不为广播地址且无报错时才回复 */
            if((ModbusPara.tBuf[0] != MB_Broadcast_ADDR) &&
                    (ERR_NOT == ModbusPara.sERR))
            {
                ModbusSend(byteCount);   /* 回复 */
            }
#ifdef DEBUG_MODBUS
            printd("\r s:");
            for(uint8_t i = 0; i < byteCount; i++)
                printd(" %02x", ModbusPara.tBuf[i]);
#endif
        }
        else
        {
            ModbusPara.sERR = ERR_MB_ADDR;  /* 操作地址无效 */
        }
    }
    else
    {
        /* 非法从站设备地址 */
        ModbusPara.sERR = ERR_MB_DEVICE_ADDR;
    }
}


void ModbusProces(void)
{
    if(MB_RECIVE_END == ModbusPara.sRUN)
    {
        if(LEAST_RCV_CNT < ModbusPara.rCnt)
        {
            LED_WORK = !LED_WORK;
            if(0 == ModbusCRC16(&ModbusPara.rBuf[0], ModbusPara.rCnt))
            {
#ifdef DEBUG_MODBUS
                printd("\r r:");
                for(uint8_t i = 0; i<ModbusPara.rCnt; i++)
                    printd(" %02x", ModbusPara.rBuf[i]);
#endif
                /* 第2字节 功能码 */
                /* 确认模块存在并且工作正常 */
                switch(ModbusPara.rBuf[1])
                {
                    /* 03 读功能码 */
                    case GET_HOLDING_REGT:
                        MB_ReadHoldingRegisters();
                        break;
                    /* 06 写功能码 */
                    case PRESET_HOLDING_sREGT:
                        MB_PresetSingleHoldingRegister();
                        break;
                    /* 10 复合写功能码 */
                    case PRESET_HOLDING_mREGT:
                        MB_PresetMultipleHoldingRegisters();
                        break;
                    /* 功能码错误/不支持 */
                    default:
                        ModbusPara.sERR = ERR_MB_FUN;
                        break;
                }
            }
            else
            {
                /* CRC校验失败 */
                ModbusPara.sERR = ERR_MB_DEVICE;
            }
            ModbusPara.rCnt = 0;
            Modbus_ERROR();
            ModbusPara.sRUN = MB_IDEL;
        }
        else
        {
            /* 数据长度不足 即无效数据 */
            ModbusPara.rCnt = 0;
            Modbus_ERROR();
            ModbusPara.sRUN = MB_IDEL;
        }
    }
}
