#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef _TIMER_H_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif


#define SEC     (1000)
#define DCSEC   (100)
#define mSEC    (10)

typedef struct
{
    uint16_t timeDbg;
    uint16_t timeCmd;
    uint16_t timeMilli;
    uint32_t timeWaitMill;
    uint16_t timeOut;   /* 错误指示灯 */
    uint16_t sec;
    uint32_t timePause; /* 停留时间计时器 */
}_TIMER_T;

PEXT _TIMER_T timerPara;


PEXT void TIM2_Init(uint16_t arr,uint16_t psc);
PEXT void TIM3_Init(uint16_t arr,uint16_t psc);
PEXT void TIM4_Init(uint16_t arr,uint16_t psc);
PEXT void TIM5_Init(uint16_t arr,uint16_t psc);
PEXT void TIM6_Init(uint16_t arr,uint16_t psc);
PEXT void TIM7_Init(uint16_t arr,uint16_t psc);






#undef PEXT
#endif






















