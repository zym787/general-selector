#define _SIGNAL_GLOBALS_
#include "common.h"



/*

*/
bool GettCliffSignal(void)
{
    static bool lastSta=false;
    bool bCliff=false;

    if(VALVE_OPT)
    {
        if(lastSta==false)
            bCliff = true;
        lastSta = true;
    }
    else
    {
        if(lastSta==true)
            bCliff = true;
        lastSta = false;
    }
    return bCliff;
}

/*
    排序
*/
void BubbleWord(uint16* array,uint8 len)
{
	uint8 i,j;
	uint16 temp;
	for(j=1;j<len;j++)
	{
		for(i=0;i<len-j;i++)
		{
			if(array[i]<array[i+1])
			{
				temp=array[i];
				array[i]=array[i+1];
				array[i+1]=temp;
			}
		}
	}
}


/*
    均值
*/
uint16 AverageN(uint16* array,uint8 len)
{
	uint8 i=0;
	uint32 sum=0;
	for(i=0;i<len;i++)
	{
        sum += *(array+i);
	}
    sum /= len;
    return sum;
}


/*

*/
uint8 SigSum(uint8 *array, uint8 len)
{
    uint8 sum=0;
    for(uint8 i=0; i<len; i++)
        sum += *(array+i);
    return sum;
}


uint16 SigSumU16(uint16 *array, uint8 len)
{
    uint16 sum=0;
    for(uint8 i=0; i<len; i++)
        sum += *(array+i);
    return sum;
}

/*
    扫描通道与信号分配
*/
void SignalScan(void)
{
    static uint8 pos=0;
    uint8 i=0;
    uint8 rwBuff[8]={0,0,0,0,0,0,0,0};
    switch(sig.stpScan)
    {
        case 100:
            if(MotionStatus[AXSV])
            {
                srd[AXSV].accel_count = srd[AXSV].decel_val;
                srd[AXSV].run_state = DECEL;
                sig.stpScan = 101;
            }
            else
            {
                sig.stpScan = 101;
            }
            break;
        case 101:
            if(!MotionStatus[AXSV])
            {
                VALVE_ENA = ON;
                sig.bRdPulse = true;
                AxisMoveRel(AXSV, -(int)rdc.stepRound*13/12, accel[AXSV]/2, decel[AXSV]/2, speed[AXSV]/2);
                memset(sig.pulseGap, 0, SIGNAL_CNT*2);
                memset(sig.pulseBlock, 0, SIGNAL_CNT*2);
                printd("\r\n scanning and reset buffer");
                sig.stpScan = 102;
            }
            break;
        case 102:
            if(!MotionStatus[AXSV])
            {
                sig.bRdPulse = false;
                BubbleWord(sig.pulseGap, SIGNAL_CNT);
                BubbleWord(sig.pulseBlock, SIGNAL_CNT);
                for(i=0; i<SIGNAL_CNT; i++)
                {
                    if(!*(sig.pulseGap+i))
                        break;
                }
                printd("\r\n num=%d", i);
                rwBuff[4] = sig.pulseGap[i-1]>>8;
                rwBuff[5] = sig.pulseGap[i-1];
                for(i=0; i<SIGNAL_CNT; i++)
                {
                    if(!*(sig.pulseBlock+i))
                        break;
                }
                printd("\r\n num=%d", i);
                sig.pulseBlock[i-3] = AverageN(sig.pulseBlock, i-2);
                rwBuff[0] = sig.pulseBlock[i-3]>>8;
                rwBuff[1] = sig.pulseBlock[i-3];
                rwBuff[2] = sig.pulseBlock[i-2]>>8;
                rwBuff[3] = sig.pulseBlock[i-2];
                rwBuff[6] = sig.pulseBlock[i-1]>>8;
                rwBuff[7] = sig.pulseBlock[i-1];

                printd("\r\n mainblock%d, second block%d, lit block%d, gap%d",
                sig.pulseBlock[i-3], sig.pulseBlock[i-2], sig.pulseBlock[i-1], sig.pulseGap[i-1]);
                I2CPageWrite_Nbytes(ADDR_SYMBOL, LEN_SYMBOL, rwBuff);
                printd("\r\n start inited");
                sig.stpScan = 1;
            }
            break;
        case 1:
        case 4:
            if(sig.stpScan==1)
            {
                I2CPageRead_Nbytes(ADDR_SYMBOL, LEN_SYMBOL, rwBuff);
                sig.pulseBlock[0] = rwBuff[0];
                sig.pulseBlock[0] <<= 8;
                sig.pulseBlock[0] |= rwBuff[1];
                // normal blade 8 percent
                uint32 temp=0;
                temp = sig.pulseBlock[0]*PERCENT_TOLL;
                sig.pulseBlock[3] = temp/PERCENT;

                sig.pulseBlock[1] = rwBuff[2];
                sig.pulseBlock[1] <<= 8;
                sig.pulseBlock[1] |= rwBuff[3];
                // large nick 20 percent
                temp = sig.pulseBlock[1]*PERCENT_TOLL;
                sig.pulseBlock[4] = temp/PERCENT;

                sig.pulseBlock[2] = rwBuff[6];
                sig.pulseBlock[2] <<= 8;
                sig.pulseBlock[2] |= rwBuff[7];
                // normal blade 20 percent
                temp = sig.pulseBlock[2]*PERCENT_TOLL;
                sig.pulseBlock[5] = temp/PERCENT;

                sig.pulseGap[0] = rwBuff[4];
                sig.pulseGap[0] <<= 8;
                sig.pulseGap[0] |= rwBuff[5];
                // normal Gap 20 percent
                temp = sig.pulseGap[0]*PERCENT_TOLL;
                sig.pulseGap[1] = temp/PERCENT;
            }
            VALVE_ENA = ON;
            Valve.status = VALVE_INITING;
            Valve.ErrBlinkTime = NORMAL_BLINK;
            Valve.passByOne = 0;
            Valve.bReInit = 1;
            Valve.initStep = 0;
            pos = 0;
            if(sig.stpScan==1)
            {
                Valve.bNewInit = 0;
                sig.stpScan = 2;
            }
            else
                sig.stpScan = 0;
            break;
        case 2:
            if(Valve.status==VALVE_RUN_END)
            {
                delay_ms(250);
                if(++pos<=valveFix.fix.portCnt)
                {
                    Valve.portDes = pos;
                    Valve.dir = 0XFF;
                    delay_ms(250);
                    sig.stpScan = 3;
                }
                else
                {
                    sig.sum = SigSum(sig.arrCount, valveFix.fix.portCnt);
                    I2CPageWrite_Nbytes(ADDR_SIG, LEN_SIG, sig.arrCount);
                    sig.stpScan = 4;
                }
            }
            break;
        case 3:
            if(Valve.status==VALVE_RUN_END)
            {
                sig.arrCount[pos-1] = sig.scanCount;
                sig.pulse[pos-1] = sig.basicPulse;
                printd("\r\n pos%d sig %d", pos, sig.arrCount[pos-1]);
                sig.scanCount = 0;
                sig.stpScan = 2;
            }
            break;
        case 0:
        default:
            break;
    }
}





