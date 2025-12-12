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

///E∞Ê±æ
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

///F∞Ê±æ
#ifdef A12_926
    // OUT1(PB13) OUT2(PA8) FBOUT(PA11) ERROUT(PA12)
    GPIOB->CRH &= (GPIO_Crh_P13);
    GPIOB->CRH |= (GPIO_Mode_Out_PP_50MHz_P13);
    GPIOB->ODR |= (GPIO_Pin_13);
    GPIOA->CRH &= (GPIO_Crh_P8 | GPIO_Crh_P11 | GPIO_Crh_P12);
    GPIOA->CRH |= (GPIO_Mode_Out_PP_50MHz_P8 | GPIO_Mode_Out_PP_50MHz_P11 | GPIO_Mode_Out_PP_50MHz_P12);
    GPIOA->ODR |= (GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12);
    // IO_OUT1 = 0;
    // IO_OUT2 = 0;
    // IO_FBOUT = 0;
    // IO_ERROUT = 0;

    // IN1(PB3) IN2(PB4)
    GPIOB->CRL &= (GPIO_Crl_P3 | GPIO_Crl_P4);
    GPIOB->CRL |= (GPIO_Mode_IN_PU_PD_P3 | GPIO_Mode_IN_PU_PD_P4);
#endif
}

/// A12-909  BI–¸ø’/5.0V  ‰≥ˆ0  AI–¸ø’/0V  ‰≥ˆ0  ◊¥Ã¨A
///          BI0V         ‰≥ˆ1  AI3.3-12V  ‰≥ˆ1  ◊¥Ã¨B
/// A12-906  BI –¸ø’/5.0V   ‰≥ˆ0
/// A12-926  IN1    IN2    OUT1    OUT2    FBOUT    ERROUT
///          0       0       0       0       0       0
///          1       0       1       0       0       0
void bsp_IODetect(void)
{
    if (true  == syspara.ioCtrl)
    {
#ifdef A12_909
        static uint8_t InPosition = 0; /* µ•≥Ã÷¥––ÕÍ≥…±Í÷æ */
        /// BI–¸ø’/Ω”5V  ‰≥ˆ0 ◊¥Ã¨A
        /// AI–¸ø’/Ω”0V  ‰≥ˆ0 ◊¥Ã¨A
        if (1 == IO_IN)
        {
            if (VALVE_RUN_END == Valve.status)
            {
                
#if FIRST_HOLE_IO_E_DIR == 1
                /// E1
                /// 3◊¥Ã¨C -> 2◊¥Ã¨B -> 1◊¥Ã¨A
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
                    printd("\r\n ∫ƒ ± %dms", timeCost);
                    IO_OUT = ON;
                }
                else
                {
                    Valve.portDes = 0x00;
                }
#elif FIRST_HOLE_IO_E_DIR == 2
                // E2
                /// 5◊¥Ã¨C -> 6◊¥Ã¨B -> 1◊¥Ã¨A
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
                    printd("\r\n ∫ƒ ± %dms", timeCost);
                    IO_OUT = ON;
                }
                else
                {
                    Valve.portDes = 0x00;
                }
#endif
            }
        }
        /// BIΩ”0V       ‰≥ˆ 1  ◊¥Ã¨C
        /// AIΩ”3.3-12V  ‰≥ˆ 1  ◊¥Ã¨C
        else
        {
            if (VALVE_RUN_END == Valve.status)
            {
#if FIRST_HOLE_IO_E_DIR == 1
                ///1◊¥Ã¨A -> 2◊¥Ã¨B -> 3◊¥Ã¨C
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
                    printd("\r\n ∫ƒ ± %dms", timeCost);
                    IO_OUT = OFF;
                }
                else
                {
                    Valve.portDes = 0x00;
                }
#elif FIRST_HOLE_IO_E_DIR == 2
                /// 1◊¥Ã¨A -> 6◊¥Ã¨B -> 5◊¥Ã¨C
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
                    printd("\r\n ∫ƒ ± %dms", timeCost);
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
#ifdef MUT_IOCTRL
        /// ININ    3 2 1 0
        /// Õ®µ¿    1 2 3 4
        /// IOOUT   3 2 1 0
        uint8_t IoInStatus = 0x03 & (~(IO_IN1 << 0 | IO_IN2 << 1));
        uint8_t IoOutStatus = 0;
        if (VALVE_RUN_END == Valve.status)
        {
            ///µΩŒª
            if ((IoInStatus + 1) == Valve.portCur)
            {
                IO_FBOUT = OFF;  ///“∆∂ØÕÍ≥…
                /// IO◊¥Ã¨ ‰≥ˆ
                IoOutStatus = Valve.portCur - 1;
                IO_OUT1 = (~IoInStatus & 0x01) ? ON : OFF;
                IO_OUT2 = (~IoInStatus & 0x02) ? ON : OFF;
                dbg_printf("\r\n > In Position %d=%d    IO  IN:%d %d  OUT:%d %d (C%d E%d)",
                           Valve.portCur, IoInStatus, IO_IN1, IO_IN2, IO_OUT1, IO_OUT2, IO_FBOUT, IO_ERROUT);
            }
            ///–¬◊¥Ã¨ ‰»Î
            else
            {
                IO_FBOUT = ON; /// “∆∂ØŒ¥ÕÍ≥…
                /// IO ‰»ÎºÏ≤‚
                Valve.portDes = IoInStatus + 1;
                Valve.dir = 0xFF;
                dbg_printf("\r\n > Update Position %d!=%d    IO  IN:%d %d  OUT:%d %d (C%d E%d)",
                           Valve.portCur, IoInStatus, IO_IN1, IO_IN2, IO_OUT1, IO_OUT2, IO_FBOUT, IO_ERROUT);
            }
        }
        ///±®¥Ì
        else if (VALVE_ERR == Valve.status)
        {
            IO_ERROUT = OFF;
            IO_FBOUT = ON;
            IO_OUT1 = ON;
            IO_OUT2 = ON;
        }
        ///‘À––&≥ı ºªØ
        else
        {
            IO_ERROUT = ON;
            IO_FBOUT = ON;
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
