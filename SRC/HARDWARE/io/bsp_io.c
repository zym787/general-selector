/**
 * @file      : bsp_io.c
 * @brief     : IO
 *
 * @version   : 1.0
 * @author    : Drinkto
 * @date      : Nov 14, 2025
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Nov 14, 2025 | 1.0 | Drinkto | ???IO |
 */
#include "common.h"

void bsp_IOInit(void)
{
    RCC->APB2ENR |= RCC_APB2Periph_AFIO;
    RCC->APB2ENR |= (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB);

#ifdef A12_909
    // FB OUT
    GPIOB->CRH &= (GPIO_Crh_P13);
    GPIOB->CRH |= (GPIO_Mode_Out_PP_50MHz_P13);
    GPIOB->ODR |= (GPIO_Pin_13);
    // KEY IN
    GPIOB->CRL &= (GPIO_Crl_P5);
    GPIOB->CRL |= (GPIO_Mode_IN_PU_PD_P5);
#endif

#ifdef A12_906
    // FB OUT
    GPIOB->CRH &= (GPIO_Crh_P13);
    GPIOB->CRH |= (GPIO_Mode_Out_PP_50MHz_P13);
    GPIOB->ODR |= (GPIO_Pin_13);
    // KEY IN
    GPIOB->CRH &= (GPIO_Crh_P14);
    GPIOB->CRH |= (GPIO_Mode_IN_PU_PD_P14);
#endif
}

/// A12-909  BIÐü¿Õ/5.0V Êä³ö0  AIÐü¿Õ/0V Êä³ö0  ×´Ì¬A
///          BI0V        Êä³ö1  AI3.3-12V Êä³ö1  ×´Ì¬B
/// A12-906  BI Ðü¿Õ/5.0V  Êä³ö0
void bsp_IODetect(void)
{
    static uint8_t InPosition = 0;  /* µ¥³ÌÖ´ÐÐÍê³É±êÖ¾ */
    if (true  == syspara.ioCtrl)
    {
#ifdef A12_909
        /// BIÐü¿Õ/½Ó5V Êä³ö0 ×´Ì¬A
        /// AIÐü¿Õ/½Ó0V Êä³ö0 ×´Ì¬A
        if (1 == IO_IN)
        {
            if (VALVE_RUN_END == Valve.status)
            {
                
#if FIRST_HOLE_IO_E_DIR == 1
                /// E1
                /// 3×´Ì¬C -> 2×´Ì¬B -> 1×´Ì¬A
                if (0x03 == Valve.portCur)
                {
                    InPosition = 0;
                    Valve.portDes = 0x02;
                    Valve.dir = 0xff;
                    dbg_printf("\r\n >1  %d->%d Dir:%d", Valve.portCur, Valve.portDes, Valve.dir);
                }
                else if (0x02 == Valve.portCur)
                {
                    syspara.ctrlPause = true;
                    dbg_printf("\r\n >2  Waiting %dms < %d", timerPara.timePause, syspara.pauseTime);
                    if (syspara.pauseTime <= timerPara.timePause)
                    {
                        timerPara.timePause = 0;
                        syspara.ctrlPause = false;
                        Valve.portDes = 0x01;
                        Valve.dir = 0xff;
                        dbg_printf("\r\n >3  %d->%d Dir:%d", Valve.portCur, Valve.portDes, Valve.dir);
                    }
#ifdef DEBUG
                    else if (1 == timerPara.timePause % 300)
                    {
                        dbg_printf("\r\n timer pauseTime: %d", timerPara.timePause);
                    }
#endif // DEBUG
                }
                else if (0x01 == Valve.portCur && !InPosition)
                {
                    uint32_t timeCost = syspara.timeRamp[0] + syspara.timeRamp[1] + syspara.pauseTime;
                    InPosition = 1;
                    dbg_printf("\r\n >4 In Position %d", Valve.portCur);
                    printd("\r\n ºÄÊ± %dms", timeCost);
                    IO_OUT = ON;
                }
                else
                {
                    Valve.portDes = 0x00;
                }
#elif FIRST_HOLE_IO_E_DIR == 2
                // E2
                /// 5×´Ì¬C -> 6×´Ì¬B -> 1×´Ì¬A
                if (0x05 == Valve.portCur)
                {
                    InPosition = 0;
                    Valve.portDes = 0x06;
                    Valve.dir = 0xff;
                    dbg_printf("\r\n >1  %d->%d Dir:%d", Valve.portCur, Valve.portDes, Valve.dir);
                }
                else if (0x06 == Valve.portCur)
                {
                    syspara.ctrlPause = true;
                    dbg_printf("\r\n >2  Waiting %dms < %d", timerPara.timePause, syspara.pauseTime);
                    if (syspara.pauseTime <= timerPara.timePause)
                    {
                        timerPara.timePause = 0;
                        syspara.ctrlPause = false;
                        Valve.portDes = 0x01;
                        Valve.dir = 0xff;
                        dbg_printf("\r\n >3  %d->%d Dir:%d", Valve.portCur, Valve.portDes, Valve.dir);
                    }
#ifdef DEBUG
                    else if (1 == timerPara.timePause % 300)
                    {
                        dbg_printf("\r\n timer pauseTime: %d", timerPara.timePause);
                    }
#endif // DEBUG
                }
                else if (0x01 == Valve.portCur && !InPosition)
                {
                    uint32_t timeCost = syspara.timeRamp[0] + syspara.timeRamp[1] + syspara.pauseTime;
                    InPosition = 1;
                    dbg_printf("\r\n >4 In Position %d", Valve.portCur);
                    printd("\r\n ºÄÊ± %dms", timeCost);
                    IO_OUT = ON;
                }
                else
                {
                    Valve.portDes = 0x00;
                }
#endif
            }
        }
        /// BI½Ó0V      Êä³ö 1  ×´Ì¬C
        /// AI½Ó3.3-12V Êä³ö 1  ×´Ì¬C
        else
        {
            if (VALVE_RUN_END == Valve.status)
            {
#if FIRST_HOLE_IO_E_DIR == 1
                ///1×´Ì¬A -> 2×´Ì¬B -> 3×´Ì¬C
                if (0x01 == Valve.portCur)
                {
                    InPosition = 0;
                    Valve.portDes = 0x02;
                    Valve.dir = 0xff;
                    syspara.recordTimeRamp = 1;
                    dbg_printf("\r\n >1  %d->%d Dir:%d", Valve.portCur, Valve.portDes, Valve.dir);
                }
                else if (0x02 == Valve.portCur)
                {
                    syspara.ctrlPause = true;
                    dbg_printf("\r\n >2  Waiting %dms", syspara.pauseTime);
                    if (syspara.pauseTime <= timerPara.timePause)
                    {
                        timerPara.timePause = 0;
                        syspara.ctrlPause = false;
                        Valve.portDes = 0x03;
                        Valve.dir = 0xff;
                        syspara.recordTimeRamp = 2;
                        dbg_printf("\r\n >3  %d->%d Dir:%d", Valve.portCur, Valve.portDes, Valve.dir);
                    }
#ifdef DEBUG
                    else
                    {
                        if (1 == timerPara.timePause % 300)
                        {
                            dbg_printf("\r\n pauseTime: %d", timerPara.timePause);
                        }
                    }
#endif // DEBUG
                }
                else if (0x03 == Valve.portCur && !InPosition)
                {
                    uint32_t timeCost = syspara.timeRamp[0] + syspara.timeRamp[1] + syspara.pauseTime;
                    InPosition = 1;
                    dbg_printf("\r\n >4 In Position %d", Valve.portCur);
                    printd("\r\n ºÄÊ± %dms", timeCost);
                    IO_OUT = OFF;
                }
                else
                {
                    Valve.portDes = 0x00;
                }
#elif FIRST_HOLE_IO_E_DIR == 2
                /// 1×´Ì¬A -> 6×´Ì¬B -> 5×´Ì¬C
                if (0x01 == Valve.portCur)
                {
                    InPosition = 0;
                    Valve.portDes = 0x06;
                    Valve.dir = 0xff;
                    syspara.recordTimeRamp = 1;
                    dbg_printf("\r\n >1  %d->%d Dir:%d", Valve.portCur, Valve.portDes, Valve.dir);
                }
                else if (0x06 == Valve.portCur)
                {
                    syspara.ctrlPause = true;
                    dbg_printf("\r\n >2  Waiting %dms", syspara.pauseTime);
                    if (syspara.pauseTime <= timerPara.timePause)
                    {
                        timerPara.timePause = 0;
                        syspara.ctrlPause = false;
                        Valve.portDes = 0x05;
                        Valve.dir = 0xff;
                        syspara.recordTimeRamp = 2;
                        dbg_printf("\r\n >3  %d->%d Dir:%d", Valve.portCur, Valve.portDes, Valve.dir);
                    }
#ifdef DEBUG
                    else
                    {
                        if (1 == timerPara.timePause % 300)
                        {
                            dbg_printf("\r\n pauseTime: %d", timerPara.timePause);
                        }
                    }
#endif // DEBUG
                }
                else if (0x05 == Valve.portCur && !InPosition)
                {
                    uint32_t timeCost = syspara.timeRamp[0] + syspara.timeRamp[1] + syspara.pauseTime;
                    InPosition = 1;
                    dbg_printf("\r\n >4 In Position %d", Valve.portCur);
                    printd("\r\n ºÄÊ± %dms", timeCost);
                    IO_OUT = OFF;
                }
                else
                {
                    Valve.portDes = 0x00;
                }
#endif
            }
        }
#endif
    }
}

void bsp_IORecordTimeRamp(void)
{
    if(syspara.recordTimeRamp)
    {
        syspara.timeRamp[syspara.recordTimeRamp-1] = syspara.lastTime;
    }
}
