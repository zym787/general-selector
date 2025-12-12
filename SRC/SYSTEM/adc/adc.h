/**
 * @file      : adc.h
 * @brief     : adc头文件
 *
 * @version   : 1.0
 * @author    : Drinkto
 * @date      : Dec 11, 2025
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Dec 11, 2025 | 1.0 | Drinkto | xxx |
 */

#ifndef __ADC_H_
#define __ADC_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#define SAMP_COUNT 20 /* 样本个数，表示200ms内的采样数据求平均值 */

extern uint16_t GetADC(void);
extern void ADC_Configuration(void);
extern void AdcPro(void);

#ifdef __cplusplus
}
#endif

#endif // __ADC_H_
