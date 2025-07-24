#define _VALVE_GLOBALS_
#include "common.h"

void ConfigValve(void)
{
    RCC->APB2ENR |= (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC);

    #ifdef A12_909_A2
    //LED
    GPIOC->CRH &= (GPIO_Crh_P15);
    GPIOC->CRH |= (GPIO_Mode_Out_PP_50MHz_P15);
    //VALVE OPTO
    GPIOA->CRH &= (GPIO_Crh_P15);
    GPIOA->CRH |= (GPIO_Mode_IN_PU_PD_P15);
    GPIOA->ODR |= (GPIO_Pin_15);
    GPIOA->CRL &= (GPIO_Crl_P4 & GPIO_Crl_P5 & GPIO_Crl_P6 & GPIO_Crl_P7);
    GPIOA->CRL |= (GPIO_Mode_Out_PP_50MHz_P4 | GPIO_Mode_Out_PP_50MHz_P5 | GPIO_Mode_Out_PP_50MHz_P6 | GPIO_Mode_Out_PP_50MHz_P7);
    // ISET
    GPIOB->CRL &= (GPIO_Crl_P0);
    GPIOB->CRL |= (GPIO_Mode_IN_PU_PD_P0);
    GPIOB->ODR |= (GPIO_Pin_0);
    GPIOB->CRH &= (GPIO_Crh_P12);
    GPIOB->CRH |= (GPIO_Mode_IN_PU_PD_P12);
    GPIOB->ODR |= (GPIO_Pin_12);
    GPIOA->CRH &= (GPIO_Crh_P11);
    GPIOA->CRH |= (GPIO_Mode_IN_PU_PD_P11);
    GPIOA->ODR |= (GPIO_Pin_11);

    #endif
    #ifdef A12_906_B1
    //LED
    GPIOC->CRH &= (GPIO_Crh_P14);
    GPIOC->CRH |= (GPIO_Mode_Out_PP_50MHz_P14);
    //VALVE OPTO
    GPIOC->CRH &= (GPIO_Crh_P15);
    GPIOC->CRH |= (GPIO_Mode_IN_PU_PD_P15);
    GPIOC->ODR |= (GPIO_Pin_15);
    GPIOA->CRL &= (GPIO_Crl_P4 & GPIO_Crl_P5 & GPIO_Crl_P6 & GPIO_Crl_P7);
    GPIOA->CRL |= (GPIO_Mode_Out_PP_50MHz_P4 | GPIO_Mode_Out_PP_50MHz_P5 | GPIO_Mode_Out_PP_50MHz_P6 | GPIO_Mode_Out_PP_50MHz_P7);
    // ISET
    GPIOB->CRH &= (GPIO_Crh_P12);
    GPIOB->CRH |= (GPIO_Mode_IN_PU_PD_P12);
    GPIOB->ODR |= (GPIO_Pin_12);
    GPIOB->CRL &= (GPIO_Crl_P0);
    GPIOB->CRL |= (GPIO_Mode_IN_PU_PD_P0);
    GPIOB->ODR |= (GPIO_Pin_0);
    GPIOA->CRH &= (GPIO_Crh_P11);
    GPIOA->CRH |= (GPIO_Mode_IN_PU_PD_P11);
    GPIOA->ODR |= (GPIO_Pin_11);

    #endif
    VALVE_RST = 0;
    delay_ms(10);
    VALVE_RST = 1;

    srd[AXXN].signalDIR = &VALVE_DIR;
    srd[AXXN].signalCLK = &VALVE_CLK;
    srd[AXXN].signalCCR1 = &TIM4->CCR1;
    srd[AXXN].signalARR = &TIM4->ARR;
    srd[AXXN].signalCR1 = &TIM4->CR1;
    //-----------------------------------------------------------------------------
    srd[AXSV].SearchOrg = ValveLimitDetect;
    srd[AXSV].bEmgStop = NULL;
}


void getPrePort(void)
{
    if(Valve.serialPort[0]||Valve.serialPort[1]||Valve.serialPort[2]||Valve.serialPort[3])
    {
        if(Valve.serialPort[0])
        {
            Valve.portDes = Valve.serialPort[0];
            Valve.serialPort[0] = 0;
        }
        else if(Valve.serialPort[1])
        {
            Valve.portDes = Valve.serialPort[1];
            Valve.serialPort[1] = 0;
        }
        else if(Valve.serialPort[2])
        {
            Valve.portDes = Valve.serialPort[2];
            Valve.serialPort[2] = 0;
        }
        else if(Valve.serialPort[3])
        {
            Valve.portDes = Valve.serialPort[3];
            Valve.serialPort[3] = 0;
        }
    }
    else
    {
        Valve.serialNum = 0;
    }
}


/*

*/
void InitValve(void)
{
    if(Valve.status&VALVE_INITING && !MotionStatus[AXSV])
    {
        if(Valve.bReInit)
        {
            if(Valve.retryTms<RETRY_TIMES)
            {
                printd("\r\n start round");
                VALVE_ENA = ENABLE;
//                ++Valve.retryTms;
                position[AXSV] = HomePos[AXSV];     // 清除当前的计步
                AxisMoveAbs(AXSV, -(int)rdc.stepRound*3, accel[AXSV], decel[AXSV], speed[AXSV]);
                Valve.status |= VALVE_RUNNING;                  // 置位运行标志
            }
            else
            {
                Valve.ErrBlinkTime = RETRY_TIME_OUT;
                VALVE_ENA = DISABLE;
                Valve.status = VALVE_ERR;
                Valve.bReInit = 0;
                printd("\r\n Inited retry time out");
            }
        }
    }
}


/*
    初始化完成后进行通道寻位，预给出两圈的行程，肯定会找到两次目标位置
    找到一个通道点，位置加1，直到找到目标位置，激活急停
    如果超过一圈没有找到目标位置，启动重新初始化，重新初始化的次数超过3次，报错退出
*/
void ProcessValve(void)
{
    float tpFloat=0;
    if(!(Valve.status&VALVE_INITING))
	{
        if(!MotionStatus[AXSV])
        {
            if(Valve.status==VALVE_RUN_END)
        	{
                //BEGIN:
                if(Valve.portCur!=Valve.portDes && Valve.portDes && Valve.portDes<=valveFix.fix.portCnt)
        	    {
                    Valve.OptBlock = 0;
                    Valve.OptGap = 0;
                    if(Valve.dir==0xff)
                    {// 就近原则妹位
                        // 找出最近方向
                        int Positive=0, Negative=0;     // 正向与反向差值
                        if(Valve.bHalfSeal)
                        {
                            if(Valve.bNewInit==1)
                            {// 复位后首次找位置，需要把1号或者10号的半通道步数补上
                                if(Valve.portDes>valveFix.fix.portCnt/2)
                                {
                                    Valve.portCur = 1;
                                    printd("\r\n reset to 1");
                                }
                                else
                                {
                                    Valve.portCur = valveFix.fix.portCnt;
                                    printd("\r\n reset to cnt");
                                }
                            }
                        }
                        if(Valve.portDes>Valve.portCur)
                        {
                            Positive = Valve.portDes-Valve.portCur;
                            Negative = Valve.portCur-Valve.portDes+valveFix.fix.portCnt;
                        }
                        else
                        {
                            Positive = Valve.portDes-Valve.portCur+valveFix.fix.portCnt;
                            Negative = Valve.portCur-Valve.portDes;
                        }
                        if(Positive<Negative)
                        {
                            Valve.dir = CCW;
                            if(Valve.portDes==valveFix.fix.portCnt)
                            {
                                VALVE_ENA = ON;
                                Valve.status = VALVE_INITING;
                                Valve.ErrBlinkTime = NORMAL_BLINK;
                                Valve.passByOne = 0;
                                Valve.initStep = 0;
                                Valve.bReInit = 1;
                            }
                            printd("\r\n short CCW");
                        }
                        else if(Positive>=Negative)
                        {
                            Valve.dir = CW;
                            printd("\r\n short CW");
                        }
                        tpFloat = rdc.stepRound;
                        (Valve.dir==CCW)?(tpFloat *= -1):(tpFloat);
                        // 清空计数，避免数据暂留
//                        Valve.OptBlock = 0;
//                        Valve.OptGap = 0;
                        AxisMoveRel(AXSV, (int)tpFloat, accel[AXSV]*2, decel[AXSV]*2, speed[AXSV]);
                        Valve.dirLast = Valve.direct;
                    }
                    else
                    {// 指定方向寻位
                        if(Valve.bHalfSeal)
                        {
                            if(Valve.bNewInit==1)
                            {// 复位后首次找位置，需要把1号或者10号的半通道步数补上
                                if(Valve.dir==CCW)
                                {
                                    Valve.portCur = valveFix.fix.portCnt;
                                    printd("\r\n reset to cnt");
                                }
                                else
                                {
                                    Valve.portCur = 1;
                                    printd("\r\n reset to 1");
                                }
                            }
                        }
                        if(Valve.dir==CCW)
                        {
                            printd("\r\n CCW");
                            if(Valve.portDes==valveFix.fix.portCnt)
                            {
                                VALVE_ENA = ON;
                                Valve.status = VALVE_INITING;
                                Valve.ErrBlinkTime = NORMAL_BLINK;
                                Valve.passByOne = 0;
                                Valve.initStep = 0;
                                Valve.bReInit = 1;
                            }
                        }
                        else
                        {
                            printd("\r\n CW");
                        }
                        tpFloat = rdc.stepRound;
                        (Valve.dir==CCW)?(tpFloat *= -1):(tpFloat);
                        // 清空计数，避免数据暂留
//                        Valve.OptBlock = 0;
//                        Valve.OptGap = 0;
                        AxisMoveRel(AXSV, (int)tpFloat, accel[AXSV]*2, decel[AXSV]*2, speed[AXSV]);
                        Valve.dirLast = Valve.direct;
                    }
                    Valve.status &= ~VALVE_RUN_END;     /* 清除运行结束标志 */
                    Valve.status |= VALVE_RUNNING;      /* 置位运行标志 */
                    Valve.statusLast = VALVE_RUNNING;
                    syspara.protectTimeOut = 0;
                    printd("\r\n %s initstep:%d (%d) ststus:%02x", 
                        __FUNCTION__, Valve.initStep, syspara.protectTimeOut, Valve.status);
                }
                else
                {
                    getPrePort();
                }
        	}
            else
            {
                if(Valve.statusLast==VALVE_RUNNING)
                {
                    if(Valve.bGetPort==1)
                    {
                        Valve.bGetPort = 0;
                        printd("\r\n get port");
                        Valve.portCur = Valve.portDes;
                        Valve.portDes = 0;
                        Valve.retryTms = 0;
                        Valve.statusLast = 0;
                        Valve.status = VALVE_RUN_END;
                        getPrePort();
                    }
                    else
                    {
						if(Valve.status!=VALVE_ERR)
                        {
                            Valve.portDes = 0;
                            Valve.ErrBlinkTime = RETRY_TIME_OUT;
                            Valve.status = VALVE_ERR;
                            VALVE_ENA = DISABLE;
                            printd("\r\n Valve ERR");
                            return;
                        }
                    }
                }
            }
        }
    }
    else if(Valve.bHalfSeal)
    {
        if(Valve.bNewInit==0xff && Valve.status&VALVE_RUN_END)
        {
            tpFloat = (float)rdc.stepRound/valveFix.fix.portCnt;
            tpFloat /= 2;
            if(!MotionStatus[AXSV])
            {
                AxisMoveRel(AXSV, -(int)tpFloat, accel[AXSV], decel[AXSV], speed[AXSV]);
                Valve.status &= ~(VALVE_INITING|VALVE_RUNNING);
                Valve.bNewInit = 1;
                Valve.bReInit = 1;
            }
        }
    }
}

/*
    原点与端口光耦的信号激时，分别激活急停功能，确保停止的位置够精确
*/
//#define PULSE_CNT_EN
void ValveLimitDetect(void)
{
    ++sig.basicPulse;
    if(!VALVE_OPT)
    {// 缺口
        ++Valve.OptGap;
        if(Valve.OptBlock)
        {// 此时处理挡片
#ifdef PULSE_CNT_EN
            printd("\r\nB%d", Valve.OptBlock);
#endif
            if(sig.bRdPulse==true)
                sig.pulseBlock[sig.num] = Valve.OptBlock;
            else
            {
                /* 较大的特征挡片后必是位置定位孔 */
                /* 反转时第二大特征挡片后必是位置定位孔 */
                if((Valve.OptBlock > (sig.pulseBlock[0]-sig.pulseBlock[3]) && 
                    Valve.OptBlock < (sig.pulseBlock[0]+sig.pulseBlock[3])) || 
                   (srd[AXSV].dir == CCW && 
                    Valve.OptBlock > (sig.pulseBlock[1]-sig.pulseBlock[4]) && 
                    Valve.OptBlock < (sig.pulseBlock[1]+sig.pulseBlock[4])))
                {
                    Valve.initStep = 0;
                    if(!(Valve.status&VALVE_INITING))
                    {
                        Valve.bPassPort = 1;
                        if(Valve.dir==CCW)
                        {
                            if(++Valve.portCur>valveFix.fix.portCnt)
                            {
                                Valve.portCur = 1;
                            }
                        }
                        else
                        {
                            if(--Valve.portCur==0)
                            {
                                Valve.portCur = valveFix.fix.portCnt;
                            }
                        }
                        if(Valve.portCur==Valve.portDes)
                        {
                            Valve.bGetPort = 1;
                            if(srd[0].dir==CW)
                                srd[0].accel_count = -Valve.fDirCw*rdc.stepP01dgr;
                            else
                                srd[0].accel_count = -Valve.fDirCCw*rdc.stepP01dgr;
                            srd[0].run_state = DECEL;
                        }
                    }
                }
                else if(Valve.OptBlock > (sig.pulseBlock[2]-sig.pulseBlock[5]) && 
                        Valve.OptBlock <= (sig.pulseBlock[2]+sig.pulseBlock[5]))
                {
                    /* 每圈进行数据清除，保证长期转动下来不会有误差累积 */
                    Valve.stpCnt = 0;
                    if(!(Valve.status&VALVE_INITING))
                    {
                        Valve.portCur = valveFix.fix.portCnt;
                        if(Valve.portCur==Valve.portDes)
                        {
                            Valve.bGetPort = 1;
                            if(srd[0].dir==CW)
                                srd[0].accel_count = -Valve.fDirCw*rdc.stepP01dgr;
                            else
                                srd[0].accel_count = -Valve.fDirCCw*rdc.stepP01dgr;
                            srd[0].run_state = DECEL;
                        }
                    }
                    else if(srd[AXSV].dir==CW)
                    {// 2步，碰到大的挡片|非初始化情况下，只碰到大缺口也可以激活原点
                        Valve.initStep = 2;
                    }
                }
                else if(Valve.OptBlock > sig.pulseBlock[0]*valveFix.fix.portCnt/4)
                {
                    Valve.portDes = 0;
                    Valve.ErrBlinkTime = RETRY_TIME_OUT;
                    Valve.status = VALVE_ERR;
                    VALVE_ENA = DISABLE;
                }
                /* 半通道处理逻辑 */
                if(Valve.bHalfSeal)
                {
                    if(!(Valve.status&VALVE_INITING)&&Valve.bNewInit==1)
                    {
                        if(Valve.dir==CCW)
                        {
                            if(++Valve.portCur>valveFix.fix.portCnt)
                            {
                                Valve.portCur = 1;
                            }
                        }
                        else
                        {
                            if(--Valve.portCur==0)
                            {
                                Valve.portCur = valveFix.fix.portCnt;
                            }
                        }
                        if(Valve.portCur==Valve.portDes)
                        {
                            Valve.bGetPort = 1;
                            if(srd[0].dir==CW)
                                srd[0].accel_count = -Valve.fDirCw*rdc.stepP01dgr;
                            else
                                srd[0].accel_count = -Valve.fDirCCw*rdc.stepP01dgr;
                            srd[0].run_state = DECEL;
                        }
                        Valve.bNewInit = 0;
                    }
                }
                else
                {
                    if(!(Valve.status&VALVE_INITING)&&Valve.bNewInit==1)
                    {
                        Valve.bNewInit = 0;
                    }
                }
            }
            syspara.OptBlockLast = Valve.OptBlock;
        }
        Valve.OptBlock = 0;
    }
    else
    {// 挡片
        ++Valve.OptBlock;
        if(Valve.OptGap)
        {// 此处处理缺口
#ifdef PULSE_CNT_EN
            printd("\r\nG%d",Valve.OptGap);
#endif
            if(sig.bRdPulse==true)
                sig.pulseGap[sig.num++] = Valve.OptGap;
            else
            {
                if(Valve.OptGap>(sig.pulseGap[0]-sig.pulseGap[1]) && Valve.OptGap<(sig.pulseGap[0]+sig.pulseGap[1]))
                {
                    if(!Valve.initStep && srd[AXSV].dir==CW)
                    {// 1步，首先碰到小的挡片
                        Valve.initStep = 1;
                    }
                }
            }
        }
        Valve.OptGap = 0;
    }
    if(sig.bRdPulse==false)
    {
        if(Valve.initStep==2)
        {
            ++Valve.stpCnt;
            if(Valve.stpCnt>Valve.fixOrg*rdc.stepP01dgr)       // 此处为１号口原点的补偿值
            {
                Valve.stpCnt = 0;
                Valve.initStep = 3;
                Valve.portCur = valveFix.fix.portCnt;
                if(Valve.status&VALVE_INITING || Valve.portDes==valveFix.fix.portCnt)
                {// 初始化完成，找到10号位原点
                    Valve.retryTms = 0;
                    srd[0].accel_count = -DEG_DECCEL*rdc.stepP01dgr;
                    position[0]= -DEG_DECCEL*rdc.stepP01dgr;
                    srd[0].run_state = DECEL;
                    if(Valve.bHalfSeal)
                    {
                        if(Valve.status&VALVE_INITING && Valve.bNewInit==0xff)
                        {
                            Valve.portCur = 0xff;
                        }
                        else if(Valve.status&VALVE_INITING || Valve.portDes==valveFix.fix.portCnt)
                        {
                            Valve.status &= ~(VALVE_INITING|VALVE_RUNNING);
                            Valve.portCur = valveFix.fix.portCnt;
                        }
                    }
                    else
                    {
                        if(Valve.status&VALVE_INITING && Valve.bNewInit==0xff)
                            Valve.bNewInit = 1;
                        Valve.status &= ~(VALVE_INITING|VALVE_RUNNING);
                        Valve.portCur = valveFix.fix.portCnt;
                    }
                    try.times = 3;
                    Valve.portDes = 0;
                    Valve.dirLast = CCW;
                    Valve.passByOne = 0;
                    Valve.bReInit = 0;
                    Valve.status &= ~VALVE_RUNNING;
                    Valve.status |= VALVE_RUN_END;
                    // 清时间，保证不会连续复位转动
                    timerPara.timeMilli = 0;
                    getPrePort();
                    speed[AXSV] = 100;
                    accel[AXSV] = 100;
                    decel[AXSV] = 200;
                    speed[AXSV] *= (Valve.spd);    /* 恢复设定速度 */
                    speed[AXSV] *= (rdc.rate);
                    accel[AXSV] *= (Valve.spd);
                    accel[AXSV] *= (rdc.rate);
                    decel[AXSV] *= (Valve.spd);
                    decel[AXSV] *= (rdc.rate);
                    printd("\r\n Restore motion speed  (%d) spd%d acc%d dec%d", 
                        Valve.spd, speed[AXSV], accel[AXSV], decel[AXSV]);
//                    printd("\r\n inited");
                }
            }
        }
    }
}


/*
    设置地址为64号的时候，模块会自动启动烧机测试模式
*/
void TestBurn(void)
{
    static uint32 bDir=0;
    if(ModbusPara.mAddrs==ADDR_MAX)
    {
        if(timerPara.timeWaitMill>intCtrl*SEC)
        {// 30秒间隔，启动模块运转到下一个通道
            timerPara.timeWaitMill = 0;
            if(Valve.status==VALVE_RUN_END)
            {
                if(Valve.portCur==0xff)
                    Valve.portDes = 1;
                if(!bDir && Valve.portCur==valveFix.fix.portCnt)
                {
                    (!bDir)?(bDir=1):(bDir);
                }
                else if(!bDir && Valve.portCur<valveFix.fix.portCnt)
                {
                    Valve.portDes = Valve.portCur+1;
                    if(Valve.portDes==valveFix.fix.portCnt)
                        bDir = 1;
                }
                else if(bDir && Valve.portCur>1)
                {
                    Valve.portDes = Valve.portCur-1;
                    if(Valve.portDes==1)
                        bDir = 0;
                }
                printd("\r\n ->%d", Valve.portDes);
            }
        }
    }
}
