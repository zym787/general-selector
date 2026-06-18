#define _USFUNC_GLOBALS_
#include "common.h"


char bRw;
unsigned char FuncIndex;
void (*FuncPtr)(char rw);          //函数功能指针
_TAB_T TermTab[];


/*

*/
void TermNone(char rw)
{
     printd("\r\n Command \"%s\" invalid!", str.rcvStr);
}


/*

*/
void TermVR(char rw)
{
    int getInt;
    if(rw==READ_ACT)
    {
        printd("\r\n %s", SOFT_VER_C);
    }
    else
    {
        unsigned char ret = FetchInt(2, 0,str.rcvStr, &getInt);
        if(!ret)
        {
            str.fUsIRF = getInt;
        }
    }
}

/*
 * 显示所有指令
 */
void TermList(char rw)
{
    printd("\r\n %s", S_LIST_M);
 }


/*

*/
void TermMap(char rw)
{
    char getChar[2][P_LEN+1];
    int getInt=0;
    int *getAddr=NULL;

    memset(getChar, 0, sizeof(getChar));
    if(rw==READ_ACT)
    {
        printd("\r\n No para");
    }
    else
    {
        unsigned char ret = FetchChar(3, 0, str.rcvStr, *getChar);
        if(ret)
        {
            printd("\r\n Err code %d", ret);
            return;
        }
        getAddr = (int *)strtohex(*getChar);
        if(**(getChar+1))
        {//判断是否有参数写入
            getInt = str2int(*(getChar+1));
            *getAddr = getInt;
            printd("\r\n Wr 0x%08x to 0x%08x", getInt, getAddr);
        }
        else
        {//无参数，取指
            getInt = (int)(*getAddr);
            printd("\r\n Rd 0x[%02x %02x %02x %02x] from 0x%08x",
                (char)(getInt>>24), (char)(getInt>>16), (char)(getInt>>8), (char)(getInt>>0), getAddr);
        }
    }
}


/*

*/
void TermIIC(char rw)
{
    if(rw==READ_ACT)
    {
        printd("\r\n No para");
    }
    else
    {
        #if FLASH_CHK
        unsigned char rwBuf[4]={0, 0, 0, 0};
        int getInt=0;

        unsigned char ret = FetchInt(3, 0, str.rcvStr, &getInt);
        if(ret)
        {
            printd("\r\n ERR%d", ret);
            return;
        }
        if(getInt)
        {
            *(rwBuf+0) = getInt>>8;
            *(rwBuf+1) = getInt;
            I2CPageWrite_Nbytes(0, 2, rwBuf);
            printd("\r\n I2C write 0x%02x 0x%02x", *(rwBuf+0), *(rwBuf+1));
            *(rwBuf+2) = 0;
            *(rwBuf+3) = 0;
            I2CPageRead_Nbytes(0, 2, rwBuf+2);
            printd("\r\n I2C read 0x%02x 0x%02x", *(rwBuf+2), *(rwBuf+3));
            if(*(rwBuf+0)==*(rwBuf+2) && *(rwBuf+1)==*(rwBuf+3))
                printd("\r\n I2C R/W Succeed!");
            else
                printd("\r\n I2C R/W Fail!");
        }
        else
        {
            *(rwBuf+0) = 0;
            *(rwBuf+1) = 0;
            I2CPageWrite_Nbytes(ADDR_BOARD_ID, LEN_BOARD_ID, rwBuf);
            printd("\r\n Clear store id!");
        }
        #else
        unsigned short rwBuf[4]={0, 0, 0, 0};
        int getInt=0;
        unsigned char ret = FetchInt(3, 0, str.rcvStr, &getInt);
        if(ret)
        {
            printd("\r\n ERR%d", ret);
            return;
        }
        if(getInt)
        {
            *(rwBuf+0) = getInt>>16;
            *(rwBuf+1) = getInt;
            STMFLASH_Write(FLASH_SAVE_ADDR, rwBuf, 2);
            printd("\r\n write 0x%04x 0x%04x", *(rwBuf+0), *(rwBuf+1));
            *(rwBuf+2) = 0;
            *(rwBuf+3) = 0;
            STMFLASH_Read(FLASH_SAVE_ADDR, rwBuf+2, 2);
            printd("\r\n read 0x%04x 0x%04x", *(rwBuf+2), *(rwBuf+3));
            if(*(rwBuf+0)==*(rwBuf+2) && *(rwBuf+1)==*(rwBuf+3))
                printd("\r\n R/W Succeed!");
            else
                printd("\r\n R/W Fail!");
        }
        else
        {
            *(rwBuf+0) = 0;
            *(rwBuf+1) = 0;
            STMFLASH_Read(ADDR_FLASH_ID, rwBuf, LEN_FLASH_ID);
            printd("\r\n Factory set!");
        }
        #endif
    }
}


void TermCat(char rw)
{
    int getInt[2] = {0, 0};
    if(rw==READ_ACT)
    {
        printd("\r\n Cat comm");
    }
    else
    {
        unsigned char ret = FetchInt(3, 0, str.rcvStr, getInt);
        printd("\r\n Cat %d %d", *(getInt+0), *(getInt+1));
       if(ret)
        {
            printd("\r\n Err code %d", ret);
            return;
        }
        switch(*(getInt+0))
        {
            case 1:

                break;
            case 2:

                break;
            case 3:

                break;
            case 4:

                break;
            default:
                break;
        }
    }
}


void TermFetch(char rw)
{
    int getInt[2] = {0, 0};
    if(rw==READ_ACT)
    {
        printd("\r\n Cat comm");
    }
    else
    {
        unsigned char ret = FetchInt(3, 2, str.rcvStr, getInt);
        if(ret)
        {
            printd("\r\n Err code %d", ret);
            return;
        }
        printd("\r\n Fet %d %d", *(getInt+0), *(getInt+1));
        switch(*(getInt+0))
        {
            case 1:

                break;
            case 2:

                break;
            case 3:

                break;
            case 4:

                break;
            default:
                break;
        }
    }
}


/*

*/
void TermTs(char rw)
{
    int getInt[2]={0, 0};
//    uint8_t buffer[2]={0,0};
    if(rw == READ_ACT)
    {
        printd("%s", S_LIST_SH);
        printd("%s", S_LIST_SBD);
        printd("%s", S_LIST_SE);
    }
    else
    {
        unsigned char ret = FetchInt(2, 0, str.rcvStr, getInt);
        if(ret)
        {
            printd("\r\n Err code %d", ret);
            return;
        }
        switch(getInt[0])
        {
            case 0:
                printd("\r\n Move axis %d rounds", getInt[1]);
                if(!MotionStatus[AXSV])
                    AxisMoveRel(AXSV, -(int)rdc.stepRound*getInt[1], accel[AXSV], decel[AXSV], speed[AXSV]);
                break;
            case 1:
                Valve.portDes = getInt[1];
                break;
            case 2:
                if(getInt[1]<=64)
                {
                    ags_mbParam.mAddrs = getInt[1];
                    printd("\r\n Set addrs %d", getInt[1]);
                    I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, (uint8_t *)&getInt[1]);
                }
                break;
            case 59686681:
                valveFix.fix.portCnt = getInt[1];
                I2CPageWrite_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
                printd("\r\n portCnt%d", valveFix.fix.portCnt);
                break;
            default:
                break;
        }
    }
}



/*

*/
void TermPos(char rw)
{
    int getInt[2] = {0,0};
    if(rw == READ_ACT)
    {
        /* 就近切换下一通道 */
        Valve.dir = 0xFF;
        if(valveFix.fix.portCnt == Valve.portCur)
        {
            Valve.portDes = 1;
        }
        else
        {
            Valve.portDes = Valve.portCur + 1;
        }
        printd("\r\n 就近:%d==>%d", Valve.portCur, Valve.portDes);
    }
    else
    {
        unsigned char ret = FetchInt(3, 0, str.rcvStr, getInt);
        if(ret)
        {
            printd("\r\n Err code %d", ret);
            return;
        }
        if(Valve.status==VALVE_RUN_END)
        {
            Valve.portDes = getInt[0];
            Valve.dir = getInt[1];
            printd("\r\n %d==>%d", Valve.portCur, getInt[0]);
            if(getInt[1]==255)
                printd(" 就近");
        }
    }
}



/*

*/
void TermFix(char rw)
{
    int getInt[2] = {0, 0};
    if(rw == READ_ACT)
    {
//        I2CPageRead_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &valveFix.array[valveFix.fix.portCnt-1]);
//        printd("\r Fix:%d", valveFix.array[valveFix.fix.portCnt-1]);
        I2CPageRead_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
        printd("\r\n Fix:%d", Valve.fixOrg);
        I2CPageRead_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
        printd("\r\n Dir:%d", valveFix.fix.dirGap);
    }
    else
    {
        unsigned char ret = FetchInt(3, 0, str.rcvStr, getInt);
        if(ret)
        {
            printd("\r\n Err code %d", ret);
            return;
        }
        switch(getInt[0])
        {
            case 0:
//                valveFix.array[valveFix.fix.portCnt-1] = getInt[1];
//                I2CPageWrite_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &valveFix.array[valveFix.fix.portCnt-1]);
                Valve.fixOrg = getInt[1];
                I2CPageWrite_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
                break;
            case 1:
                valveFix.fix.dirGap = getInt[1];
                I2CPageWrite_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
                break;
            default:
                break;
        }
    }
}



/*

*/
void TermAddr(char rw)
{
    int getInt=0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &ags_mbParam.mAddrs);
        printd("\r\n 读取地址 %d", ags_mbParam.mAddrs);
    }
    else
    {
        unsigned char ret = FetchInt(4, 0, str.rcvStr, &getInt);
        if(ret)
        {
            printd("\r\n Err code %d", ret);
            return;
        }
        if(AGS_ADDR_MIN <= getInt && BURN_ADDR >= getInt)
        {
            ags_mbParam.mAddrs = getInt;
            printd("\r\n 设置地址 %d", ags_mbParam.mAddrs);
        }
        else
        {
            printd("\r\n %d 地址超出范围 (合法地址:%d-%d)", getInt, AGS_ADDR_MIN, BURN_ADDR);
            ags_mbParam.mAddrs = AGS_ADDR_DEF;
            printd("\r\n 使用默认地址 %d", ags_mbParam.mAddrs);
        }
        modbus.Address = ags_mbParam.mAddrs;
        I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &ags_mbParam.mAddrs);
    }
}


/*

*/
void TermInt(char rw)
{
    int getInt=0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_INTVL, LEN_INTVL, &syspara.agingInterval);
        printd("\r\n 读取老化间隔 %d 秒", syspara.agingInterval);
    }
    else
    {
        unsigned char ret = FetchInt(3, 0, str.rcvStr, &getInt);
        if(ret)
        {
            printd("\r\n Err code %d", ret);
            return;
        }
        if(getInt&&getInt<=255)
        {
            printd("\r\n 设置老化间隔 %d 秒", getInt);
            syspara.agingInterval = getInt;
            I2CPageWrite_Nbytes(ADDR_INTVL, LEN_INTVL, &syspara.agingInterval);
        }
    }
}

/*
 * 速度设置
 * SPD=速度,初始化速度。单指令是仅修改速度
 */
void TermSpd(char rw)
{
    int getInt[2]={0,0};
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
        I2CPageRead_Nbytes(ADDR_SPD_INIT, LEN_SPD_INIT, &Valve.spdInit);
        printd("\r\n 读取速度: %d转/分   初始化速度: %d转/分", Valve.spd, Valve.spdInit);
    }
    else
    {
        unsigned char ret = FetchInt(3, 0, str.rcvStr, getInt);
        if(ret)
        {
            printd("\r\n Err code %d", ret);
            return;
        }

        printd("\r\n 设置速度和初始化速度\r\n SPD=n,m  n:速度 m:初始化速度\r\n SPD=n  n:速度");

        if (tBoundary.spd_min <= getInt[0] && tBoundary.spd_max >= getInt[0]) {
                Valve.spd = getInt[0];
                printd("\r\n 参数1: 设置速度 %d转/分", Valve.spd);
        } else {
                printd("\r\n %d 速度超限 (%d减速比速度范围: %d-%d)", getInt[0], rdc.rate, tBoundary.spd_min,
                       tBoundary.spd_max);
                Valve.spd = tBoundary.spd_init;
                printd("\r\n 参数1: 使用默认速度 %d", Valve.spd);
        }

        if (tBoundary.spd_min <= getInt[1] && tBoundary.spd_max >= getInt[1]) {
                Valve.spdInit = getInt[1];
                printd("\r\n 参数2: 设置初始化速度 %d转/分", Valve.spdInit);
        } else {
                printd("\r\n %d 初始化速度超限 (%d减速比初始化速度范围: %d-%d)", getInt[1], rdc.rate, tBoundary.spd_min,
                       tBoundary.spd_max);
                Valve.spdInit = INIT_SPD;
                printd("\r\n 参数2: 使用默认初始化速度 %d", Valve.spdInit);
        }
        
        I2CPageWrite_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
        I2CPageWrite_Nbytes(ADDR_SPD_INIT, LEN_SPD_INIT, &Valve.spdInit);
    }
}


/*

*/
void TermSN(char rw)
{
    int getInt[LEN_SN]={0};
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
        printd("\r\n 读序列号:");
        for (uint8_t i = 0; i < LEN_SN; i++)
          printd(" %02x", Valve.SnCode[i]);
    }
    else
    {
        unsigned char ret = FetchInt(2, 0, str.rcvStr, getInt);
        if(ret)
        {
            printd("\r\n Err code %d", ret);
            return;
        }

        printd("\r\n 设置序列号: ");
        I2CPageWrite_Nbytes(ADDR_SN, LEN_SN, (uint8_t*)getInt);
        for (uint8_t i = 0; i < LEN_SN; i++)
          printd(" %02x", Valve.SnCode[i]);
    }
}

/*

*/
void TermProtocal(char rw)
{
    int getInt=0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &syspara.protocol_type);
        printd("\r\n now protocol is");
        if(AGS_MODBUS == syspara.protocol_type)
        {
            printd(" AGS");
        }
        else if(EXT_COMM == syspara.protocol_type)
        {
            printd(" HX");
        }
        else if(MODBUS == syspara.protocol_type)
        {
            printd(" MODBUS");
        }
        else
        {
            printd(" wrong type");
        }
    } else {
            unsigned char ret = FetchInt(5, 0, str.rcvStr, &getInt);
            if (ret) {
                    printd("\r\n Err code %d", ret);
                    return;
            }
#if (defined FIRST_HOLE_C) || (defined END_HOLE_D)
            switch (getInt) {
                    default:
                            printd("\r\n wrong type set default AGS");
                    case AGS_MODBUS:
                            syspara.protocol_type = AGS_MODBUS;
                            printd("\r\n set protocal to AGS");
                            break;
                    case EXT_COMM:
                            syspara.protocol_type = EXT_COMM;
                            printd("\r\n set protocal to HX");
                            break;
                    case MODBUS:
                            syspara.protocol_type = MODBUS;
                            printd("\r\n set protocal to MODBUS");
                            break;
            }
#else
            printd("\r\n 仅支持AGS");
            syspara.protocol_type = AGS_MODBUS;
#endif
            I2CPageWrite_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &syspara.protocol_type);
    }
}



/*

*/
void TermBaud(char rw)
{
    int getInt = 0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.baudrate);
        printd("\r 波特率:%d %dbps", syspara.baudrate, BaudRate_V[syspara.baudrate]);
    }
    else
    {
        unsigned char ret = FetchInt(3, 0, str.rcvStr, &getInt);
        if(ret)
        {
            printd("\r\n Err code %d", ret);
            return;
        } 
        if(9600 == getInt)
        {
                syspara.baudrate = BAUD_9600;
        }
        else if(19200 == getInt)
        {
                syspara.baudrate = BAUD_19200;
        }
        else if(38400 == getInt)
        {
                syspara.baudrate = BAUD_38400;
        }
        else
        {
            printd("\r\n %d 波特率超出范围", getInt);
            return;
        }
        printd("\r\n 设置波特率为 %d %dbps", syspara.baudrate, getInt);
        I2CPageWrite_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.baudrate);
    }
}

/*

*/
void TermPulse(char rw)
{
#if 0
    if(rw == READ_ACT)
    {
        
        if(syspara.bRdPulse==false)
            syspara.bRdPulse = true;
        else
            syspara.bRdPulse = false;
        I2CPageWrite_Nbytes(ADDR_RDP, LEN_RDP, &syspara.bRdPulse);
    }
#endif
}

/*

*/
void TermScan(char rw)
{
    if(rw == READ_ACT)
    {
        sig.stpScan = 100;
        sig.num = 0;
        printd("\r\n 开始信号扫描");
    }
}


/*
 * 电机顺时针减速值
 * 阀头逆时针
*/
void TermDirCw(char rw)
{
    int getInt = 0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_DIR_SD, LEN_DIR_SD-1, (uint8_t *)&getInt);
        printd("\r\n 逆时针减速值:%d", getInt);
    }
    else
    {
        unsigned char ret = FetchInt(5, 0, str.rcvStr, &getInt);
        if(ret)
        {
            printd("\r Err code %d", ret);
            return;
        }
        if(getInt<=255)
        {
            I2CPageWrite_Nbytes(ADDR_DIR_SD, LEN_DIR_SD-1, (uint8_t *)&getInt);
            Valve.fDirCw = getInt;
            printd("\r\n 设置逆时针减速值:%d", getInt);
        }
    }
}

/*
 * 电机逆时针减速值
 * 阀头顺时针
 */
void TermDirCCw(char rw)
{
    int getInt = 0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_DIR_SD+1, LEN_DIR_SD-1, (uint8_t *)&getInt);
        printd("\r\n 顺时针减速值:%d", getInt);
    }
    else
    {
        unsigned char ret = FetchInt(6, 0, str.rcvStr, &getInt);
        if(ret)
        {
            printd("\r Err code %d", ret);
            return;
        }
        if(getInt<=255)
        {
            I2CPageWrite_Nbytes(ADDR_DIR_SD+1, LEN_DIR_SD-1, (uint8_t *)&getInt);
            Valve.fDirCCw = getInt;
            printd("\r\n 设置顺时针减速值:%d", getInt);
        }
    }
}

/*

*/
void TermReset(char rw)
{
    if(rw == READ_ACT)
    {
        Valve.status = VALVE_INITING;
        Valve.initStep = 0;
        Valve.bNewInit = 0xff;
        Valve.passByOne = 0;
        Valve.bReInit = 1;
        Valve.goFirstFlag = 0;
        I2CPageRead_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
        (valveFix.fix.portCnt&&valveFix.fix.portCnt>32)?(valveFix.fix.portCnt=10):(valveFix.fix.portCnt);
        I2CPageRead_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
        I2CPageRead_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
        printd("\r\n 复位... 请10秒后操作设备");
    }
}

/*

*/
void TermCnt(char rw)
{
    int getInt = 0;
    unsigned char ret = FetchInt(3, 0, str.rcvStr, &getInt);
    if(ret)
    {
        printd("\r Err code %d", ret);
        return;
    }
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
        printd("\r\n 读通道数:%d", valveFix.fix.portCnt);
    }
    else
    {
        if(CHANNEL_MIN <= getInt && CHANNEL_MAX >= getInt)
        {
            valveFix.fix.portCnt = getInt;
            printd("\r\n 设置通道数:%d", valveFix.fix.portCnt);
        }
        else
        {
            printd("\r\n %d 通道数超出范围 (%d-%d)", getInt, CHANNEL_MIN, CHANNEL_MAX);
            valveFix.fix.portCnt = CHANNEL_DEF;
            printd("\r\n 使用默认通道数 %d", valveFix.fix.portCnt);
        }
        I2CPageWrite_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
    }
}

/*

*/
void TermRDCR(char rw)
{
    int getInt = 0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_RDC_RATE, LEN_RDC_RATE, &rdc.rate);
        printd("\r\n 读取减速比 %d", rdc.rate);
    }
    else
    {
        unsigned char ret = FetchInt(4, 0, str.rcvStr, &getInt);
        if(ret)
        {
            printd("\r Err code %d", ret);
            return;
        }        
        if (RDCR_1 == getInt || RDCR_4 == getInt || RDCR_10 == getInt || 
            RDCR_16 == getInt || RDCR_20 == getInt)
        {
            rdc.rate = getInt;
            printd("\r\n 设置减速比 %d", rdc.rate);
        }
        else
        {
            rdc.rate = RDCR_10;
            printd("\r\n %d 减速比超出范围 使用默认减速比 %d", getInt, rdc.rate);
        }
        I2CPageWrite_Nbytes(ADDR_RDC_RATE, LEN_RDC_RATE, &rdc.rate);
    }
}

/*

*/
void TermHalf(char rw)
{
    int getInt = 0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_HALF_SEAL, LEN_HALF_SEAL, &Valve.bHalfSeal);
        printd("\r\n 读取半通道 :%d  %s", Valve.bHalfSeal,
                (Valve.bHalfSeal) == 0 ? "关闭" : "开启");
    }
    else
    {
        unsigned char ret = FetchInt(4, 0, str.rcvStr, &getInt);
        if(ret)
        {
            printd("\r Err code %d", ret);
            return;
        }
        if(0 == getInt || 1 == getInt)
        {
            printd("\r\n 设置半通道 :%d  %s", getInt, 
                    (getInt) == 0 ? "关闭" : "开启");
            Valve.bHalfSeal = getInt;
            
        }
        else
        {
            printd("\r\n 非0值必须强制设置为1\
                    \r\n 开启半通道");
            Valve.bHalfSeal = 1;
        }
        I2CPageWrite_Nbytes(ADDR_HALF_SEAL, LEN_HALF_SEAL, &Valve.bHalfSeal);
    }
}

/*
 * 停留时间
 */
void TermPauseTime(char rw)
{
    int getInt = 0;
    if (rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_PAUSE_TIME, LEN_PAUSE_TIME, (uint8_t*)&syspara.pauseTime);
        printd("\r\n 停留时间 %d毫秒", syspara.pauseTime);
    }
    else
    {
        unsigned char ret = FetchInt(6, 0, str.rcvStr, &getInt);
        if (ret)
        {
            printd("\r Err code %d", ret);
            return;
        }
        if (0 <= getInt && ADZ_PT_MAX >= getInt)
        {
            syspara.pauseTime = getInt;
            printd("\r\n 写入停留时间 %d毫秒", syspara.pauseTime);
        }
        else
        {
            syspara.pauseTime = 0;
            printd("\r\n %d 停留时间超限 写入默认值0 %d (0-%d)", getInt, syspara.pauseTime, ADZ_PT_MAX);
        }
        I2CPageWrite_Nbytes(ADDR_PAUSE_TIME, LEN_PAUSE_TIME, (uint8_t*)&syspara.pauseTime);
    }
}

/*
 * IO 
 */
void TermIO(char rw)
{
    if (rw == READ_ACT)
    {
        syspara.ioCtrl = !syspara.ioCtrl;
        I2CPageWrite_Nbytes(ADDR_IO_CTRL, LEN_IO_CTRL, (uint8_t *)&syspara.ioCtrl);
        printd("\r IO控制:%d %s", syspara.ioCtrl, (0 == syspara.ioCtrl ? "关闭" : "开启"));
    }
}

/*
 * 点检模式：打印出所有关键参数
 */
void TermInspection(char rw)
{
        printd("\r\n ------------------------------------------");
        printd("\r\n                 请 注 意 !                ");
        printd("\r\n      未 使 用 变 量 请 勿 自 行 识 别      ");
        printd("\r\n ------------------------------------------\r\n");
        printd("\r\n ***************< 点检模式 >***************");
        /* 点检参数 */
        printd("\r\n 版本       (VR)   : %s", SOFT_VER_C);                                               /* 版本号 */
        printd("\r\n 电路板     (PCB)  : %s", PCB_VR);                                                   /* PCB版本号 */
        printd("\r\n 编译时间   (TIME) : %s %s", __DATE__, __TIME__);                                    /* 时间 */
        printd("\r\n 地址       (ADDR) : %d", ags_mbParam.mAddrs);                                       /* 地址 */
        printd("\r\n 通道数     (CNT)  : %d", valveFix.fix.portCnt);                                     /* 通道数 */
        printd("\r\n 波特率     (BAUD) : %d %dbps", syspara.baudrate, BaudRate_V[syspara.baudrate]);     /* 波特率 */
        printd("\r\n 速度       (SPD)  : %d RPM", Valve.spd);                                            /* 速度 */
        // printd("\r\n 初始化速度 (SPD)  : %d RPM", Valve.spdInit);                                        /* 初始化速度 */
        printd("\r\n 减速比     (RDCR) : %d", rdc.rate);                                                 /* 减速比 */
        printd("\r\n 半通道     (HALF) : %d %s", Valve.bHalfSeal, (0 == Valve.bHalfSeal ? "关" : "开")); /* 半通道 */
        printd("\r\n 逆时针补偿 (CW)   : %d", Valve.fDirCw);  /* 顺时针补偿 */
        printd("\r\n 顺时针补偿 (CCW)  : %d", Valve.fDirCCw); /* 逆时针补偿 */
        printd("\r\n 原点补偿   (FIXO) : %d", Valve.fixOrg);  /* 原点补偿 */
        // printd("\r\n 方向补偿   (FIXG) : %d", valveFix.fix.dirGap);      /* 方向补偿 */
        // printd("\r\n 老化次数  (TESTC) : %d", syspara.burnCnt);          /* 老化次数 */
        printd("\r\n 切换次数  (MOVES) : %d", syspara.totalCnt); /* 切换次数 */
#ifdef FIRST_HOLE_IO_E
    printd("\r\n IO控制     (IOE) : %d %s", syspara.ioCtrl, (0 == syspara.ioCtrl ? "关" : "开")); /* IO */
    printd("\r\n 停留时间   (REPLY): %d", syspara.pauseTime);        /* 停留时间 */
#endif // FIRST_HOLE_IO_E
#ifdef FIRST_HOLE_MUT_IO_F
    printd("\r\n IO控制     (IOE) : %d %s", syspara.ioCtrl, (0 == syspara.ioCtrl ? "关" : "开")); /* IO */
    printd("\r\n                         1  2  3  4");
    printd("\r\n 切换顺序     (STATC)  : %d %d %d %d", Valve.StatusChannel[0], Valve.StatusChannel[1], Valve.StatusChannel[2], Valve.StatusChannel[3]);                            /* IO */
#endif // FIRST_HOLE_MUT_IO_F
    /* 序列号 */
    printd("\r\n 序列号     (SN)   : %02X %02X %02X %02X %02X", 
        Valve.SnCode[0], Valve.SnCode[1], Valve.SnCode[2], Valve.SnCode[3], Valve.SnCode[4]);
    printd("\r\n ***************< 点检模式 >***************\r\n");
}

/*
 * 老化次数
 */
void TermTestCnt(char rw)
{
    int getInt = 0;
    uint32_t saveCnt = 0;
    if (rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_BURN_CNT, LEN_BURN_CNT, (uint8_t *)&saveCnt);
        if(saveCnt <= syspara.burnCnt)
        {
            I2CPageWrite_Nbytes(ADDR_BURN_CNT, LEN_BURN_CNT, (uint8_t *)&syspara.burnCnt);
            dbg_printf("Saved");
        }
        printd("\r\n 老化次数 %d", syspara.burnCnt);
    }
    else
    {
        unsigned char ret = FetchInt(5, 0, str.rcvStr, &getInt);
        if (ret)
        {
            printd("\r Err code %d", ret);
            return;
        }
        printd("\r\n 设置老化次数 %d", getInt);
        syspara.burnCnt = getInt;
        I2CPageWrite_Nbytes(ADDR_BURN_CNT, LEN_BURN_CNT, (uint8_t *)&syspara.burnCnt);
    }
}

/*
 * 通道状态
 */
void TermStateChannel(char rw)
{
#ifdef MUT_IOCTRL
    int getInt[4] = {1, 1, 1, 1};
    if (rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_STATE_CHANNEL, LEN_STATE_CHANNEL, Valve.StatusChannel);
        printd("\r\n                1  2  3  4");
        printd("\r\n 读取通道状态:");
        for (uint8_t i = 0; i < 4; ++i)
            printd(" %d", *(Valve.StatusChannel + i));
        printd("\r\n");
    }
    else
    {
        unsigned char ret = FetchInt(5, 0, str.rcvStr, getInt);
        if (ret)
        {
            printd("\r\n Err code %d", ret);
            return;
        }
        printd("\r\n                1  2  3  4");
        printd("\r\n 设置通道状态: ");
        ///范围检查
        for (uint8_t i = 0; i < 4; i++)
        {
            if (1 <= getInt[i] && getInt[i] <= valveFix.fix.portCnt)
            {
                Valve.StatusChannel[i] = getInt[i];
                printd(" %d", Valve.StatusChannel[i]);
            }
            else
            {
                printd("\r\n 错误! 第%d个通道值%d超限 请重新检查", i + 1, Valve.StatusChannel[i]);
                return;
            }
        }
        I2CPageWrite_Nbytes(ADDR_STATE_CHANNEL, LEN_STATE_CHANNEL, Valve.StatusChannel);
    }
#else
        printd("\r\n 该版本不支持通道状态功能");
#endif
}

/*
 * 切换次数
 */
void TermMovesCnt(char rw)
{
    int getInt = 0;
    printd("\r\n func %s", __func__);
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_TOTAL_CNT, LEN_TOTAL_CNT, ((uint8_t*)&syspara.totalCnt));
        printd("\r 切换次数:%d", syspara.totalCnt);
    }
    else
    {
        unsigned char ret = FetchInt(5, 0, str.rcvStr, &getInt);
        if(ret)
        {
            printd("\r Err code %d", ret);
            return;
        }
        syspara.totalCnt = getInt;
        printd("\r 写入切换次数:%d", syspara.totalCnt);
        I2CPageWrite_Nbytes(ADDR_TOTAL_CNT, LEN_TOTAL_CNT, (uint8_t*)&syspara.totalCnt);
    }
}


//-------------------------界面相关定制函数-------------------------//
_TAB_T TermTab[]=
{
    {0,     (*TermNone)},
    {1,     (*TermList)},
    {2,     (*TermVR)},
    {3,     (*TermMap)},
    {4,     (*TermIIC)},
    {5,     (*TermTs)},
    {6,     (*TermCat)},
    {7,     (*TermFetch)},
    {8,     (*TermPos)},
    {9,     (*TermFix)},
    {10,    (*TermAddr)},
    {11,    (*TermInt)},
    {12,    (*TermSpd)},
    {13,    (*TermSN)},
    {14,    (*TermProtocal)},
    {15,    (*TermBaud)},
    {16,    (*TermPulse)},
    {17,    (*TermScan)},
    {18,    (*TermDirCw)},
    {19,    (*TermDirCCw)},
    {20,    (*TermReset)},
    {21,    (*TermCnt)},
    {22,    (*TermRDCR)},
    {23,    (*TermHalf)},
    {24,    (*TermInspection)},
    {25,    (*TermPauseTime)},
    {26,    (*TermIO)},
    {27,    (*TermTestCnt)},
    {28,    (*TermStateChannel)},
    {29,    (*TermMovesCnt)},
};

/*

*/
void ChRUN(char *cmdName)
{
    if(!strcasecmp(cmdName, CMD_LIST))
    {
        FuncIndex = 1;
    }
    else if(!strncasecmp(cmdName, CMD_VR, 2))
    {
        (!strcasecmp(cmdName, CMD_VR))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 2;
    }
    else if(!strcasecmp(cmdName, CMD_MAP) || !strncasecmp(cmdName, CMD_MAP, 3))
    {
        (!strcasecmp(cmdName, CMD_MAP))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 3;
    }
    else if(!strcasecmp(cmdName, CMD_IIC) || !strncasecmp(cmdName, CMD_IIC, 3))
    {
        (!strcasecmp(cmdName, CMD_IIC))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 4;
    }
    else if(!strncasecmp(cmdName, CMD_TS, 2))
    {
        (!strcasecmp(cmdName, CMD_TS))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 5;
    }
    else if(!strcasecmp(cmdName, CMD_CAT) || !strncasecmp(cmdName, CMD_CAT, 3))
    {
        (!strcasecmp(cmdName, CMD_CAT))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 6;
    }
    else if(!strcasecmp(cmdName, CMD_FET) || !strncasecmp(cmdName, CMD_FET, 3))
    {
        (!strcasecmp(cmdName, CMD_FET))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 7;
    }
    else if(!strcasecmp(cmdName, CMD_POS) || !strncasecmp(cmdName, CMD_POS, 3))
    {
        (!strcasecmp(cmdName, CMD_POS))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 8;
    }
    else if(!strcasecmp(cmdName, CMD_FIX) || !strncasecmp(cmdName, CMD_FIX, 3))
    {
        (!strcasecmp(cmdName, CMD_FIX))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 9;
    }
    else if(!strcasecmp(cmdName, CMD_ADDR) || !strncasecmp(cmdName, CMD_ADDR, 4))
    {
        (!strcasecmp(cmdName, CMD_ADDR))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 10;
    }
    else if(!strcasecmp(cmdName, "INT") || !strncasecmp(cmdName, "INT", 3))
    {
        (!strcasecmp(cmdName, "INT"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 11;
    }
    else if(!strcasecmp(cmdName, "SPD") || !strncasecmp(cmdName, "SPD", 3))
    {
        (!strcasecmp(cmdName, "SPD"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 12;
    }
    else if(!strcasecmp(cmdName, "SN") || !strncasecmp(cmdName, "SN", 2))
    {
        (!strcasecmp(cmdName, "SN"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 13;
    }
    else if(!strcasecmp(cmdName, "PRTCL") || !strncasecmp(cmdName, "PRTCL", 5))
    {
        (!strcasecmp(cmdName, "PRTCL"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 14;
    }
    else if(!strcasecmp(cmdName, "BDR") || !strncasecmp(cmdName, "BDR", 3))
    {
        (!strcasecmp(cmdName, "BDR"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 15;
    }
    else if(!strcasecmp(cmdName, "RDP") || !strncasecmp(cmdName, "RDP", 3))
    {
        (!strcasecmp(cmdName, "RDP"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 16;
    }
    else if(!strcasecmp(cmdName, "SCAN") || !strncasecmp(cmdName, "SCAN", 4))
    {
        (!strcasecmp(cmdName, "SCAN"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 17;
    }
    else if(!strcasecmp(cmdName, "DIRCW") || !strncasecmp(cmdName, "DIRCW", 5))
    {
        (!strcasecmp(cmdName, "DIRCW"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 18;
    }
    else if(!strcasecmp(cmdName, "DIRCCW") || !strncasecmp(cmdName, "DIRCCW", 6))
    {
        (!strcasecmp(cmdName, "DIRCCW"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 19;
    }
    else if(!strcasecmp(cmdName, "RESET") || !strncasecmp(cmdName, "RESET", 5))
    {
        (!strcasecmp(cmdName, "RESET"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 20;
    }
    else if(!strcasecmp(cmdName, "CNT") || !strncasecmp(cmdName, "CNT", 3))
    {
        (!strcasecmp(cmdName, "CNT"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 21;
    }
    else if(!strcasecmp(cmdName, "RDCR") || !strncasecmp(cmdName, "RDCR", 4))
    {
        (!strcasecmp(cmdName, "RDCR"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 22;
    }
    else if(!strcasecmp(cmdName, "HALF") || !strncasecmp(cmdName, "HALF", 4))
    {
        (!strcasecmp(cmdName, "HALF"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 23;
    }
    else if(!strcasecmp(cmdName, "INSP") || !strncasecmp(cmdName, "INSP", 4))
    {
        (!strcasecmp(cmdName, "INSP"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 24;
    }
    else if(!strcasecmp(cmdName, "PAUSET") || !strncasecmp(cmdName, "PAUSET", 5))
    {
        (!strcasecmp(cmdName, "PAUSET"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 25;
    }
    else if(!strcasecmp(cmdName, "IOE") || !strncasecmp(cmdName, "IOE", 3))
    {
        (!strcasecmp(cmdName, "IOE"))?(bRw = READ_ACT):(bRw = WRITE_ACT);
        FuncIndex = 26;
    }
    else if (!strcasecmp(cmdName, "TESTC") || !strncasecmp(cmdName, "TESTC", 5))
    {
        (!strcasecmp(cmdName, "TESTC")) ? (bRw = READ_ACT) : (bRw = WRITE_ACT);
        FuncIndex = 27;
    }
    else if (!strcasecmp(cmdName, "STATC") || !strncasecmp(cmdName, "STATC", 5))
    {
        (!strcasecmp(cmdName, "STATC")) ? (bRw = READ_ACT) : (bRw = WRITE_ACT);
        FuncIndex = 28;
    }
    else if (!strcasecmp(cmdName, "MOVES") || !strncasecmp(cmdName, "MOVES", 5))
    {
        (!strcasecmp(cmdName, "MOVES")) ? (bRw = READ_ACT) : (bRw = WRITE_ACT);
        FuncIndex = 29;
    }
    else
    {
        FuncIndex = 0;
    }
    FuncPtr = TermTab[FuncIndex].CurrOp;
    (*FuncPtr)(bRw);
}


