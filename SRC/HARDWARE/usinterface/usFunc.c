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
//    uint8 buffer[2]={0,0};
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
                    ModbusPara.mAddrs = getInt[1];
                    printd("\r\n Set addrs %d", getInt[1]);
                    I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, (uint8 *)&getInt[1]);
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
        printd("\r\n Nearby:%d==>%d", Valve.portCur, Valve.portDes);
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
                printd(" short");
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
        I2CPageRead_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &ModbusPara.mAddrs);
        printd("\r\n Addr:%d", ModbusPara.mAddrs);
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
            ModbusPara.mAddrs = getInt;
            printd("\r\n Set Addr to %d", ModbusPara.mAddrs);
        }
        else
        {
            printd("\r\n %d Address out of range (%d-%d)", getInt, AGS_ADDR_MIN, BURN_ADDR);
            ModbusPara.mAddrs = AGS_ADDR_DEF;
            printd("\r\n Use default Address %d", ModbusPara.mAddrs);
        }
        I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &ModbusPara.mAddrs);
    }
}


/*

*/
void TermInt(char rw)
{
    int getInt=0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_INTVL, LEN_INTVL, &intCtrl);
        printd("\r Interval:%d Sec", intCtrl);
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
            printd("\r\n set Interval to %d Sec", getInt);
            intCtrl = getInt;
            I2CPageWrite_Nbytes(ADDR_INTVL, LEN_INTVL, &intCtrl);
        }
    }
}

/*

*/
void TermSpd(char rw)
{
    int getInt=0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
        printd("\r\n read Spd %d RPM", Valve.spd);
    }
    else
    {
        unsigned char ret = FetchInt(3, 0, str.rcvStr, &getInt);
        if(ret)
        {
            printd("\r\n Err code %d", ret);
            return;
        }
        
        if(tBoundary.spd_min <= getInt && tBoundary.spd_max >= getInt)
        {
            Valve.spd = getInt;
            printd("\r\n set speed to %d", Valve.spd);
        }
        else
        {
            printd("\r\n %d Speed out of range (%dRDCR: %d-%d)", 
                getInt, rdc.rate, tBoundary.spd_min, tBoundary.spd_max);
            Valve.spd = tBoundary.spd_min;
            printd("\r\n Use default Speed %d", Valve.spd);
        }
        I2CPageWrite_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
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
        printd("\r\n read SN:");
        for(uint8 i=0; i<10; i++)
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

        printd("\r\n set sn code");
        I2CPageWrite_Nbytes(ADDR_SN, LEN_SN, (uint8*)getInt);
    }
}

/*

*/
void TermProtocal(char rw)
{
    int getInt=0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &syspara.typeProtocal);
        if(MY_MODBUS == syspara.typeProtocal)
        {
            printd("\r\n AGS");
        }
        else if(EXT_COMM == syspara.typeProtocal)
        {
            printd("\r\n EXTCOM");
        }
        else
        {
            printd("\r\n read wrong type");
        }
    }
    else
    {
        unsigned char ret = FetchInt(5, 0, str.rcvStr, &getInt);
        if(ret)
        {
            printd("\r\n Err code %d", ret);
            return;
        }
        if(getInt==MY_MODBUS || getInt==EXT_COMM)
        {
            syspara.typeProtocal = getInt;
            printd("\r\n set protocal to");
            if(syspara.typeProtocal==MY_MODBUS)
                printd("\r\n AGS");
            else
                printd("\r\n EXTCOM");
            I2CPageWrite_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &syspara.typeProtocal);
        }
        else
        {
            printd("\r\n wrong type  0:AGS  1:EXT");
        }
    }
}



/*

*/
void TermBaud(char rw)
{
    int getInt = 0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.bdrate);
        printd("\r\n Baud:%d", syspara.bdrate);
    }
    else
    {
        unsigned char ret = FetchInt(3, 0, str.rcvStr, &getInt);
        if(ret)
        {
            printd("\r\n Err code %d", ret);
            return;
        }
        if(getInt%9600)
        {
            printd("\r\n baud rate %d not exist", getInt);
            return;
        }
        
        if(9600 == getInt)
        {
            syspara.bdrate = 1;
        }
        else if(19200 == getInt)
        {
            syspara.bdrate = 2;
        }
        else if(38400 == getInt)
        {
            syspara.bdrate = 3;
        }
        else
        {
            printd("\r\n baud rate %d overflow", getInt);
            return;
        }
        printd("\r set baud rate to %d %dbps", syspara.bdrate, getInt);
        I2CPageWrite_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.bdrate);
    }
}

/*

*/
void TermPulse(char rw)
{
    if(rw == READ_ACT)
    {
        if(syspara.bRdPulse==false)
            syspara.bRdPulse = true;
        else
            syspara.bRdPulse = false;
        I2CPageWrite_Nbytes(ADDR_RDP, LEN_RDP, &syspara.bRdPulse);
    }
}

/*

*/
void TermScan(char rw)
{
    if(rw == READ_ACT)
    {
        sig.stpScan = 100;
        sig.num = 0;
        printd("\r\n start signal scan");
    }
}


/*

*/
void TermDirCw(char rw)
{
    int getInt = 0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_DIR_SD, LEN_DIR_SD-1, (uint8 *)&getInt);
        printd("\r\n 顺时针减速值:%d", getInt);
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
            I2CPageWrite_Nbytes(ADDR_DIR_SD, LEN_DIR_SD-1, (uint8 *)&getInt);
            Valve.fDirCw = getInt;
            printd("\r\n 设置顺时针减速值:%d", getInt);
        }
    }
}

/*

*/
void TermDirCCw(char rw)
{
    int getInt = 0;
    if(rw == READ_ACT)
    {
        I2CPageRead_Nbytes(ADDR_DIR_SD+1, LEN_DIR_SD-1, (uint8 *)&getInt);
        printd("\r\n 逆时针减速值:%d", getInt);
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
            I2CPageWrite_Nbytes(ADDR_DIR_SD+1, LEN_DIR_SD-1, (uint8 *)&getInt);
            Valve.fDirCCw = getInt;
            printd("\r\n 设置逆时针减速值:%d", getInt);
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
        I2CPageRead_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
        (valveFix.fix.portCnt&&valveFix.fix.portCnt>32)?(valveFix.fix.portCnt=10):(valveFix.fix.portCnt);
        I2CPageRead_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
        I2CPageRead_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
        printd("\r\n 复位");
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
        printd("\r\n portCnt:%d", valveFix.fix.portCnt);
    }
    else
    {
        if(CHANNEL_MIN <= getInt && CHANNEL_MAX >= getInt)
        {
            valveFix.fix.portCnt = getInt;
            printd("\r\n set Channel to %d", valveFix.fix.portCnt);
        }
        else
        {
            printd("\r\n %d Channel out of range (%d-%d)", getInt, CHANNEL_MIN, CHANNEL_MAX);
            valveFix.fix.portCnt = CHANNEL_DEF;
            printd("\r\n Use default Channel %d", valveFix.fix.portCnt);
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
        printd("\r\n read rate %d", rdc.rate);
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
            printd("\r\n set Rate to %d", rdc.rate);
        }
        else
        {
            rdc.rate = RDCR_10;
            printd("\r\n %d Rate out of range. Use default Rate %d", getInt, rdc.rate);
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
        printd("\r Half Seal:%d  %s", Valve.bHalfSeal, 
                (Valve.bHalfSeal) == 0 ? "OFF" : "ON");
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
            printd("\r\n set Half Seal:%d  %s", getInt, 
                    (getInt) == 0 ? "OFF" : "ON");
            Valve.bHalfSeal = getInt;
            
        }
        else
        {
            printd("\r\n Non-zero valuse MUST BE forced to 1\
                    \r\n ENABLE Half Seal");
            Valve.bHalfSeal = 1;
        }
        I2CPageWrite_Nbytes(ADDR_HALF_SEAL, LEN_HALF_SEAL, &Valve.bHalfSeal);
    }
}

/*
 * 点检模式：打印出所有关键参数
 */
void TermInspection(char rw)
{
    printd("\r\n");
    /* 点检参数 */
    printd("\r\n VR   : %s", SOFT_VER_C);               /* 版本号 */
    printd("\r\n PCB  : %s", PCB_VR);                   /* PCB版本号 */
    printd("\r\n TIME : %s %s", __DATE__, __TIME__);    /* 时间 */
    printd("\r\n ADDR : %d", ModbusPara.mAddrs);        /* 地址 */
    printd("\r\n CNT  : %d", valveFix.fix.portCnt);     /* 通道数 */
    printd("\r\n BAUD : %d", syspara.bdrate);           /* 波特率 */
    printd("\r\n SPD  : %d", Valve.spd);                /* 速度 */
    printd("\r\n RDCR : %d", rdc.rate);                 /* 减速比 */
    printd("\r\n HALF : %d", Valve.bHalfSeal);          /* 半通道 */
    printd("\r\n CW   : %d", Valve.fDirCw);
    printd("\r\n CCW  : %d", Valve.fDirCCw);
    printd("\r\n FIXO : %d", Valve.fixOrg);         /* 原点补偿 */
    printd("\r\n FIXG : %d", valveFix.fix.dirGap);      /* 方向补偿 */
    /* 序列号 */
    printd("\r\n SN   : %02X %02X %02X %02X %02X", 
        Valve.SnCode[0], Valve.SnCode[1], Valve.SnCode[2], Valve.SnCode[3], Valve.SnCode[4]);
    printd("\r\n");
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
    else
    {
        FuncIndex = 0;
    }
    FuncPtr = TermTab[FuncIndex].CurrOp;
    (*FuncPtr)(bRw);
}


