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

/// IO????
#ifdef A12_909
#define IO_OUT PBout(13)
#define IO_IN PBin(5)
#endif
#ifdef A12_906
#define IO_OUT PBout(13)
#define IO_IN PBin(14)
#endif

extern void bsp_IOInit(void);
extern void bsp_IODetect(void);
extern void bsp_IORecordTimeRamp(void);

#endif // !__BSP_IO_H
