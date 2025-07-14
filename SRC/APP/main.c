#define _MAIN_H_GLOBALS_
#include "common.h"

uint8 moduleAddrDflt = 0, valveFixDflt = 0, valveFixDir=0, valvePortCnt=10, IntDflt=5, SpdDflt=3, protocalDflt=MY_MODBUS;
uint8 bRdpDflt = 0;

void ParameterInit(void)
{
    uint8 ReadBuf[8]={0,0,0,0,0,0,0,0};

    I2CPageRead_Nbytes(ADDR_BOARD_ID, LEN_BOARD_ID, ReadBuf);
    if(ReadBuf[0]==0x88 && ReadBuf[1]==0x66)
    {
        printd("\r Read stored data");

        I2CPageRead_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &ModbusPara.mAddrs);
        printd("\r Addr:%d", ModbusPara.mAddrs);

        I2CPageRead_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
        (valveFix.fix.portCnt&&valveFix.fix.portCnt>32)?(valveFix.fix.portCnt=10):(valveFix.fix.portCnt);
        printd("\r Port Cnt:%d", valveFix.fix.portCnt);

        I2CPageRead_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
        printd("\r Fix:%d", Valve.fixOrg);

        I2CPageRead_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
        printd("\r Dir:%d", valveFix.fix.dirGap);

        printd("\r Fix:");
        for(uint32 i=0; i<valveFix.fix.portCnt; i++)
            printd(" %d", valveFix.array[i]);

        I2CPageRead_Nbytes(ADDR_INTVL, LEN_INTVL, &intCtrl);
        printd("\r Interval:%d", intCtrl);

        I2CPageRead_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
        I2CPageRead_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &syspara.typeProtocal);

        I2CPageRead_Nbytes(ADDR_RDP, LEN_RDP, &syspara.bRdPulse);
        if(syspara.bRdPulse==true)
            printd("\r\n enable pulse read");
        else
            printd("\r\n disable pulse read");
        I2CPageRead_Nbytes(ADDR_SIG, LEN_SIG, sig.arrCount);
        sig.sum = SigSum(sig.arrCount, valveFix.fix.portCnt);
        printd("\r\n SIG");
        for(uint8 i=0; i<valveFix.fix.portCnt; i++)
            printd(" %d", sig.arrCount[i]);
        I2CPageRead_Nbytes(ADDR_SYMBOL, LEN_SYMBOL, ReadBuf);
        sig.pulseBlock[0] = ReadBuf[0];
        sig.pulseBlock[0] <<= 8;
        sig.pulseBlock[0] |= ReadBuf[1];
        // lit blade 20 percent
        uint32 temp=0;
        temp = sig.pulseBlock[0]*PERCENT_TOLL;
        sig.pulseBlock[3] = temp/PERCENT;

        sig.pulseBlock[1] = ReadBuf[2];
        sig.pulseBlock[1] <<= 8;
        sig.pulseBlock[1] |= ReadBuf[3];
        // middle blade 8 percent
        temp = sig.pulseBlock[1]*PERCENT_TOLL;
        sig.pulseBlock[4] = temp/PERCENT;

        sig.pulseBlock[2] = ReadBuf[6];
        sig.pulseBlock[2] <<= 8;
        sig.pulseBlock[2] |= ReadBuf[7];
        // normal blade 20 percent
        temp = sig.pulseBlock[2]*PERCENT_TOLL;
        sig.pulseBlock[5] = temp/PERCENT;

        sig.pulseGap[0] = ReadBuf[4];
        sig.pulseGap[0] <<= 8;
        sig.pulseGap[0] |= ReadBuf[5];
        // mini Gap 8 percent
        temp = sig.pulseGap[0]*PERCENT_TOLL;
        sig.pulseGap[1] = temp/PERCENT;
        printd("\r\n Symbol %d %d %d %d %d %d %d %d",
            sig.pulseBlock[0], sig.pulseBlock[3], sig.pulseBlock[1], sig.pulseBlock[4],
            sig.pulseBlock[2], sig.pulseBlock[5], sig.pulseGap[0], sig.pulseGap[1]);

        I2CPageRead_Nbytes(ADDR_DIR_SD, LEN_DIR_SD, ReadBuf);
        Valve.fDirCw = ReadBuf[0];
        Valve.fDirCCw = ReadBuf[1];
        (!Valve.fDirCw||Valve.fDirCw>100)?(Valve.fDirCw=1):(Valve.fDirCw);
        (!Valve.fDirCCw||Valve.fDirCCw>100)?(Valve.fDirCCw=1):(Valve.fDirCCw);
        printd("\r\n 定位减速:%d %d", Valve.fDirCw, Valve.fDirCCw);
        
        I2CPageRead_Nbytes(ADDR_RDC_RATE, LEN_RDC_RATE, &rdc.rate);
		switch(rdc.rate)
		{
			case RDC01:
				rdc.stepP1dgr = STEPS_1_DEGREE_RD01;
				rdc.stepP01dgr = STEPS_01_DEGREE_RD01;
				break;
            case RDC04:
                rdc.stepP1dgr = STEPS_1_DEGREE_RD04;
                rdc.stepP01dgr = STEPS_01_DEGREE_RD04;
                break;
			case RDC10:
				rdc.stepP1dgr = STEPS_1_DEGREE_RD10;
				rdc.stepP01dgr = STEPS_01_DEGREE_RD10;
				break;
			case RDC16:
				rdc.stepP1dgr = STEPS_1_DEGREE_RD16;
				rdc.stepP01dgr = STEPS_01_DEGREE_RD16;
				break;
      defalut:
				rdc.rate = RDC10;
				rdc.stepP1dgr = STEPS_1_DEGREE_RD10;
				rdc.stepP01dgr = STEPS_01_DEGREE_RD10;
				break;
		}
		rdc.stepRound = P_ROUND;
		rdc.stepRound *= SCALE;
		rdc.stepRound *= rdc.rate;
        I2CPageRead_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
        if(!Valve.spd || Valve.spd>SPD_LMT)
            Valve.spd = SPD_VALVE;
        speed[AXSV] *= (Valve.spd);
        speed[AXSV] *= (rdc.rate);
        accel[AXSV] *= (Valve.spd);
        accel[AXSV] *= (rdc.rate);
        decel[AXSV] *= (Valve.spd);
        decel[AXSV] *= (rdc.rate);
        printd("\r Spd:%d", Valve.spd);
        I2CPageRead_Nbytes(ADDR_HALF_SEAL, LEN_HALF_SEAL, &Valve.bHalfSeal);
        printd("\r\n rate %d seal %d", rdc.rate, Valve.bHalfSeal);
    }
    else
    {
        printd("\r\n Write default data");
        ReadBuf[0] = 0x88;
        ReadBuf[1] = 0x66;
        I2CPageWrite_Nbytes(ADDR_BOARD_ID, LEN_BOARD_ID, ReadBuf);
        I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &moduleAddrDflt);
        ModbusPara .mAddrs = moduleAddrDflt;
        I2CPageWrite_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valvePortCnt);
        valveFix.fix.portCnt = valvePortCnt;
        printd("\r Port Cnt:%d", valveFix.fix.portCnt);

        I2CPageWrite_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &valveFixDflt);
        Valve.fixOrg = valveFixDflt;
        I2CPageWrite_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFixDir);
        valveFix.fix.dirGap = valveFixDir;
        I2CPageWrite_Nbytes(ADDR_INTVL, LEN_INTVL, &IntDflt);
        intCtrl = IntDflt;
        I2CPageWrite_Nbytes(ADDR_SPD, LEN_SPD, &SpdDflt);
        Valve.spd = SpdDflt;
        speed[AXSV] *= (Valve.spd);
        speed[AXSV] *= (rdc.rate);
        accel[AXSV] *= (Valve.spd);
        accel[AXSV] *= (rdc.rate);
        decel[AXSV] *= (Valve.spd);
        decel[AXSV] *= (rdc.rate);

        memset(Valve.SnCode, 0, sizeof(Valve.SnCode));
        I2CPageWrite_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
        I2CPageWrite_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &protocalDflt);

        syspara.bRdPulse = (bool)bRdpDflt;
        if(syspara.bRdPulse==true)
            printd("\r\n enable pulse read");
        else
            printd("\r\n disable pulse read");

        I2CPageRead_Nbytes(ADDR_SIG, LEN_SIG, sig.arrCount);
        sig.sum = SigSum(sig.arrCount, valveFix.fix.portCnt);
        printd("\r\n SIG");
        for(uint8 i=0; i<valveFix.fix.portCnt; i++)
            printd(" %d", sig.arrCount[i]);
    }
    VALVE_ENA = ON;
    Valve.status = VALVE_INITING;
    Valve.ErrBlinkTime = NORMAL_BLINK;
    Valve.passByOne = 0;
    Valve.bReInit = 1;
    Valve.bNewInit = 0xff;
}

/*
    GPIO初始化
*/
void GPIOInit(void)
{
    RCC->APB2ENR |= (RCC_APB2Periph_GPIOB);
    GPIOB->CRL &= (GPIO_Crl_P1);
    GPIOB->CRL |= (GPIO_Mode_Out_PP_50MHz_P1);
	RX_EN();			                        // 开机为接收模式
}

/*

*/
int main(void)
{
	Stm32_Clock_Init(9);	        //系统时钟设置
	delay_init(72);	   	 	        //延时初始化
    JTAG_Set(JTAG_SWD_DISABLE);

	Usart1_Init(72, 115200);	 	//串口初始化为115200
    iic_INIT();
    ConfigValve();
 	TIM2_Init(999,71);              //10Khz的计数频率
    TIM4_Init(65535,35);            //X轴脉冲定时器
    GPIOInit();
    delay_ms(100);
    BootInterface();
    ParameterInit();
    if(syspara.typeProtocal==MY_MODBUS)
        ModbusInit();
    else
        CommInit();
	while(1)
	{
        if(syspara.typeProtocal==MY_MODBUS)
            ModbusProces();
        else
            UsartProcess();

        InitValve();
        ProcessValve();
        SignalScan();
        TestBurn();
        DebugOut();
    }
}

void DebugOut(void)
{
    if(timerPara.timeDbg>SEC)
    {
        timerPara.timeDbg = 0;
        LED_WORK = !LED_WORK;
        printd("\r\n >>sta:0x%02x,port:0x%02x,%d,%d,%d,%d",
            Valve.status, Valve.portCur, Valve.portDes, Valve.retryTms, Valve.OptBlock, VALVE_OPT);
        if(syspara.typeProtocal==MY_MODBUS)
            printd(" MODBUS");
        else
            printd(" EXTCOM %d %d", protext.stepCnt, protext.time);
    }
    if(Valve.bPassPort)
    {
        Valve.bPassPort = 0;
        printd("\r\n P %d %d", Valve.portCur, syspara.OptBlockLast);
    }
}





