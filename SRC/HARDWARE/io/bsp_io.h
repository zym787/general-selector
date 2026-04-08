/**
 * @file      : bsp_io.h
 * @brief     : IO??????l?
 * 
 * @version   : 1.0
 * @author    : Drinkto
 * @date      : Nov 14, 2025
 * 
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Nov 14, 2025 | 1.0 | Drinkto | ??'??IO |
 */
#ifndef __BSP_IO_H
#define __BSP_IO_H

#define ADZ_PT_MAX  0x05265C00  /* 最大停留时间 */

#if ((defined IOCTRL) || (defined MUT_IOCTRL))
/// IO引脚定义
#ifdef A12_909
#define IO_OUT PBout(13)
#define IO_IN PBin(5)
#endif
#ifdef A12_906
#define IO_OUT PBout(13)
#define IO_IN PBin(14)
#endif
#ifdef A12_926
#define IO_IN1 PBin(3)
#define IO_IN2 PBin(4)
#define IO_OUT1 PBout(13)
#define IO_OUT2 PAout(8)
#define IO_FBOUT PAout(11)
#define IO_ERROUT PAout(12)
#endif
#endif

extern void bsp_IOInit(void);
extern void bsp_IODetect(void);
extern void bsp_IORecordTimeRamp(void);

#endif // !__BSP_IO_H
