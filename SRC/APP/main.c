#define _MAIN_H_GLOBALS_
#include "common.h"

uint8_t moduleAddrDflt = 1, valveFixDflt = 0, valveFixDir=0, 
    valvePortCnt=10, IntDflt=5, SpdDflt=INIT_SPD, protocalDflt=MY_MODBUS;
uint8_t bRdpDflt = 0;

void ParameterInit(void)
{
    uint8 ReadBuf[8]={0,0,0,0,0,0,0,0};

    // 读取板号判断是否第一次进行初始化
    I2CPageRead_Nbytes(ADDR_BOARD_ID, LEN_BOARD_ID, ReadBuf);
    // 读取默认参数
    if(ReadBuf[0]==0x88 && ReadBuf[1]==0x66)
    {
        printd("\r Read stored data");

        // 地址 0~63
        I2CPageRead_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &ModbusPara.mAddrs);
        printd("\r Addr:%d", ModbusPara.mAddrs);
        
        /* 波特率 */
        I2CPageRead_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.bdrate);
        printd("\r Baud:%d  %s bps", syspara.bdrate, 
            (syspara.bdrate) == 1 ? "9600" : (syspara.bdrate) == 2 ? "19200" : "Error");

        // 通道数
        I2CPageRead_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
        (valveFix.fix.portCnt&&valveFix.fix.portCnt>32)?(valveFix.fix.portCnt=10):(valveFix.fix.portCnt);
        printd("\r Port Cnt:%d", valveFix.fix.portCnt);
 
        // 原点补偿
        I2CPageRead_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
        printd("\r Fix Org:%d DEG", Valve.fixOrg);

        // 方向补偿
        I2CPageRead_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
        printd("\r Fix Dir:%d (0.1)DEG", valveFix.fix.dirGap);

        printd("\r Fix:");
        for(uint32 i=0; i<valveFix.fix.portCnt; i++)
            printd(" %d", valveFix.array[i]);

        // 烧机间隔
        I2CPageRead_Nbytes(ADDR_INTVL, LEN_INTVL, &intCtrl);
        printd("\r Interval:%d Sec", intCtrl);

        /* 序列号 */
        I2CPageRead_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
        I2CPageRead_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &syspara.typeProtocal);

        I2CPageRead_Nbytes(ADDR_RDP, LEN_RDP, &syspara.bRdPulse);
        if(syspara.bRdPulse==true)
            printd("\r\n enable pulse read");
        else
            printd("\r\n disable pulse read");
        I2CPageRead_Nbytes(ADDR_SIG, LEN_SIG, sig.arrCount);
        sig.sum = SigSum(sig.arrCount, valveFix.fix.portCnt);
        printd("\r\n SIG:");
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
        printd("\r\n 定位减速:CW%d CCW%d", Valve.fDirCw, Valve.fDirCCw);

        /* 减速比 */
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
            default:
                printd("\r 减速比参数错误,缺省写入%d", RDC10);
                rdc.rate = RDC10;
                rdc.stepP1dgr = STEPS_1_DEGREE_RD10;
                rdc.stepP01dgr = STEPS_01_DEGREE_RD10;
                break;
        }
        rdc.stepRound = P_ROUND;    // 单圈步数 200
        rdc.stepRound *= SCALE;     // 细分
        rdc.stepRound *= rdc.rate;  // 减速比
        printd("\r Rate:%d Round:%d", rdc.rate, rdc.stepRound);
        /* 速度 */
        I2CPageRead_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
        if(!Valve.spd || Valve.spd>SPD_LMT)
            Valve.spd = INIT_SPD;
        printd("\r Speed:%d RPM", Valve.spd);
        /* 半通道 */
        I2CPageRead_Nbytes(ADDR_HALF_SEAL, LEN_HALF_SEAL, &Valve.bHalfSeal);
        printd("\r Half Seal:%d %s", Valve.bHalfSeal, 
                (Valve.bHalfSeal) == 0 ? "OFF" : "ON");
    }
    else
    {
        printd("\r\n Write default data");
        /* 板号 */
        ReadBuf[0] = 0x88;
        ReadBuf[1] = 0x66;
        I2CPageWrite_Nbytes(ADDR_BOARD_ID, LEN_BOARD_ID, ReadBuf);
        /* 地址 1 */
        ModbusPara .mAddrs = moduleAddrDflt;
        I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &ModbusPara .mAddrs);
        /* 波特率 1 9600bps */
        syspara.bdrate = 1;
        I2CPageRead_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.bdrate);
        /* 通道数 10 */
        valveFix.fix.portCnt = valvePortCnt;
        I2CPageWrite_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
        printd("\r Port Cnt:%d", valveFix.fix.portCnt);
        /* 原点补偿 0 */
        Valve.fixOrg = valveFixDflt;
        I2CPageWrite_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
        /* 方向补偿 0 */
        valveFix.fix.dirGap = valveFixDir;
        I2CPageWrite_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
        /* CW/CCW补偿 0 */
        Valve.fDirCw = 0;
        I2CPageWrite_Nbytes(ADDR_DIR_SD, LEN_DIR_SD-1, &Valve.fDirCw);
        Valve.fDirCCw = 0;
        I2CPageWrite_Nbytes(ADDR_DIR_SD+1, LEN_DIR_SD-1, &Valve.fDirCCw);
        /* 老化间隔 5秒 */
        intCtrl = IntDflt;
        I2CPageWrite_Nbytes(ADDR_INTVL, LEN_INTVL, &intCtrl);
        /* 减速比 10 */
        rdc.rate = RDC10;
        I2CPageWrite_Nbytes(ADDR_RDC_RATE, LEN_RDC_RATE, &rdc.rate);
        /* 速度 20 */
        Valve.spd = SpdDflt;
        I2CPageWrite_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
        /* 半通道 0 */
        Valve.bHalfSeal = 0;
        I2CPageRead_Nbytes(ADDR_HALF_SEAL, LEN_HALF_SEAL, &Valve.bHalfSeal);
        /* 序列号 */
        memset(Valve.SnCode, 0, sizeof(Valve.SnCode));
        I2CPageWrite_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
        /* 协议 */
        syspara.typeProtocal = protocalDflt;
        I2CPageWrite_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &syspara.typeProtocal);
        /* 读脉冲标志 */
        syspara.bRdPulse = (bool)bRdpDflt;
        if(syspara.bRdPulse==true)
            printd("\r\n enable pulse read");
        else
            printd("\r\n disable pulse read");

        I2CPageRead_Nbytes(ADDR_SIG, LEN_SIG, sig.arrCount);
        sig.sum = SigSum(sig.arrCount, valveFix.fix.portCnt);
        printd("\r\n SIG:");
        for(uint8 i=0; i<valveFix.fix.portCnt; i++)
            printd(" %d", sig.arrCount[i]);
        printd("\r 写入成功,请复位!!!");
    }
    getOptStartStatus();
    /* 使用初始化速度找原点 */
    speed[AXSV] = 100;
    accel[AXSV] = 100;
    decel[AXSV] = 200;
    speed[AXSV] *= (INIT_SPD);
    speed[AXSV] *= (rdc.rate);
    accel[AXSV] *= (INIT_SPD);
    accel[AXSV] *= (rdc.rate);
    decel[AXSV] *= (INIT_SPD);
    decel[AXSV] *= (rdc.rate);
    printd("\r\n Init motion!  Slow Down!  (%d) spd%d acc%d dec%d", 
        INIT_SPD, speed[AXSV], accel[AXSV], decel[AXSV]);
    VALVE_ENA = ON;
    Valve.status = VALVE_INITING;
    Valve.ErrBlinkTime = NORMAL_BLINK;
    Valve.passByOne = 0;
    Valve.bReInit = 1;
    Valve.bNewInit = 0xff;
}

// 检测任务
// 1 半通道
// 2 超时检测任务(1s)
#define SINGLE_RUN_TIMEOUT          5           // 运行5秒超时
#define SINGLE_INITING_TIMOUT       14           // 转一圈差不多3秒，复位单次是两圈
void everySecDo(void)
{
    if(!Valve.bHalfSeal)
    {
    	if(!(Valve.status&VALVE_INITING)&&Valve.bNewInit==1)
    	{
    		Valve.dir = CCW;
    		Valve.portDes = 1;
    		Valve.bNewInit = 0;
    	}
	}
    // 每秒检测一次
    if(timerPara.sec > SEC)
    {
        timerPara.sec = 0;
        // 超时报错
        // 单通道间做5秒的超时处理，避免长时间堵转烧坏电路
        if((Valve.status == VALVE_RUNNING && 
            syspara.protectTimeOut > SINGLE_RUN_TIMEOUT*SEC) ||
            (Valve.status&VALVE_INITING && 
            syspara.protectTimeOut > SINGLE_INITING_TIMOUT*SEC))
        {
            if(!(Valve.status&VALVE_ERR))
            {
                Valve.portDes = 0;
                Valve.status = VALVE_ERR;
                VALVE_ENA = DISABLE;
            }
            else
            {
                VALVE_ENA = DISABLE;
            }
            printd("\r\n time out error! (initstep%d,%dms)", 
                Valve.initStep, syspara.protectTimeOut);
            Valve.ErrBlinkTime = RETRY_TIME_OUT;
        }
        // 15秒超时锁机
        if(syspara.protectTimeOut > (SINGLE_INITING_TIMOUT+1)*SEC)
        {
            Valve.status = VALVE_ERR;
            VALVE_ENA = DISABLE;
            printd("\r\n %d Timeout protection! (initstep%d,%dms)", 
                SINGLE_INITING_TIMOUT+1, Valve.initStep, syspara.protectTimeOut);
            Valve.ErrBlinkTime = RETRY_TIME_OUT;
        }
    }
}

/*
    GPIO初始化
*/
void GPIOInit(void)
{
    // FB OUT
    GPIOB->CRH &= (GPIO_Crh_P13);
    GPIOB->CRH |= (GPIO_Mode_Out_PP_50MHz_P13);
    GPIOB->ODR |= (GPIO_Pin_13);
    // KEY IN
    GPIOB->CRL &= (GPIO_Crl_P5);
    GPIOB->CRL |= (GPIO_Mode_IN_PU_PD_P5);

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
//    JTAG_Set(SWD_ENABLE);
    
	Usart1_Init(72, 115200);	 	//串口初始化为115200
    iic_INIT();
    ConfigValve();
 	TIM2_Init(999,71);              //10Khz的计数频率
    TIM4_Init(65535,35);            //X轴脉冲定时器
    GPIOInit();
    delay_ms(100);
    BootInterface();
    printd("\r\n Version:%s(%08X)  Time: %s %s \
        \r\n Description:%s (%s)\
        \r\n PCB:%s  %s \r\n", 
    SOFT_VER_C, SOFT_VER, __DATE__, __TIME__, 
    DESCRIPTION, CONTROL, 
    PCB_VR, HARDWARE_DESCRIPTION);
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
		everySecDo();
        SignalScan();
        TestBurn();
        DebugOut();
        ErrBlink();
    }
}

void DebugOut(void)
{
    if(timerPara.timeDbg > SEC * 3)
    {
        timerPara.timeDbg = 0;
        // LED_WORK = !LED_WORK;
        printd("\r\n >>sta:0x%02x  %02x->%02x  retry:%d  OptBlock:%d  Opt:%d  bNewInit:%d",
            Valve.status, Valve.portCur, Valve.portDes, Valve.retryTms, 
            Valve.OptBlock, VALVE_OPT, Valve.bNewInit);
        if(syspara.typeProtocal==MY_MODBUS)
            printd("  AGS");
        else
            printd("  EXTCOM %d %d", protext.stepCnt, protext.time);
    }
    if(Valve.bPassPort)
    {
        Valve.bPassPort = 0;
        printd("\r\n P %d %d", Valve.portCur, syspara.OptBlockLast);
    }
}

void ErrBlink(void)
{
    /* 设置led闪烁间隔 */
    if(timerPara.timeOut > Valve.ErrBlinkTime)
    {
        timerPara.timeOut = 0;
        LED_WORK = !LED_WORK;
    }
}
