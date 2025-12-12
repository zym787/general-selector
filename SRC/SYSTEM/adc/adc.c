/**
 * @file      : adc.c
 * @brief     : adc源文件
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

#include "common.h"

#ifdef MUT_IOCTRL

uint16_t g_usAdcValue; /* ADC 采样值的平均值 */


/*
*********************************************************************************************************
*	函 数 名: ADC_Configuration
*	功能说明: 配置ADC, PA0作为ADC通道输入
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void ADC_Configuration(void)
{
    // GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	__IO uint16_t ADCConvertedValue;

    ///使能时钟
    ///使能ADC1和GPIOA时钟
    RCC->APB2ENR |= (RCC_APB2Periph_ADC1);   // 使能ADC1时钟
    RCC->APB2ENR |= (RCC_APB2Periph_GPIOA);  // 使能PORTA口时钟
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);

    /* 配置PA0为模拟输入(ADC Channel0) */
    GPIOA->CRL &= GPIO_Crl_P0;
    GPIOA->CRL |= GPIO_Mode_IN_AIN;
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	// GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* 配置ADC1, 不用DMA, 用软件触发 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

    /* 配置ADC1 规则通道0 channel0 configuration */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);

    /* 使能 ADC1 DMA功能 */
    ADC_DMACmd(ADC1, ENABLE);

    /* 使能 ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /* 使能ADC1 复位校准寄存器 */
    ADC_ResetCalibration(ADC1);
    /* 检查ADC1的复位寄存器 */
    while(ADC_GetResetCalibrationStatus(ADC1));

    /* 启动ADC1校准 */
    ADC_StartCalibration(ADC1);
    /* 检查校准是否结束 */
    while(ADC_GetCalibrationStatus(ADC1));

    /* 软件启动ADC转换 */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    
//    ///使能时钟
//    ///使能ADC1和GPIOA时钟
//    RCC->APB2ENR |= (RCC_APB2Periph_ADC1);   // 使能ADC1时钟
//    RCC->APB2ENR |= (RCC_APB2Periph_GPIOA);  // 使能PORTA口时钟

//    /* 配置PA0为模拟输入(ADC Channel0) */
//    GPIOA->CRL &= GPIO_Crl_P0;
//    GPIOA->CRL |= GPIO_Mode_IN_AIN;

//    /* 配置ADC1, 不用DMA, 用软件触发 */
//    
//    ADC1->CR1 &= CR1_CLEAR_Mask;
//    /* Set DUALMOD bits according to ADC_Mode value */
//    /* Set SCAN bit according to ADC_ScanConvMode value */
//    /// ADC_Mode ADC_ScanConvMode<<8
//    ADC1->CR2 |= ADC_Mode_Independent | ENABLE << 8;

//    ADC1->CR2 &= CR2_CLEAR_Mask;
//    /* Set ALIGN bit according to ADC_DataAlign value */
//    /* Set EXTSEL bits according to ADC_ExternalTrigConv value */
//    /* Set CONT bit according to ADC_ContinuousConvMode value */
//    /// ADC_DataAlign | ADC_ExternalTrigConv | ADC_ContinuousConvMode<<1
//    ADC1->CR2 |= ADC_Mode_Independent | ADC_ExternalTrigConv_None | ENABLE << 1;

//    ADC1->SQR1 &= SQR1_CLEAR_Mask;
//    /* Set L bits according to ADC_NbrOfChannel value */
//    /// ADC_NbrOfChannel<<20
//    ADC1->SQR1 |= (1 - 1) << 20;

//    /* 配置ADC1 规则通道0 channel0 configuration */
//    ADC1->SMPR2 &= ~(SMPR2_SMP_Set << (3 * ADC_Channel_0));
//    ADC1->SMPR2 |= (SMPR2_SMP_Set << (3 * ADC_Channel_0)) | 
//                    (ADC_SampleTime_239Cycles5 << (3 * ADC_Channel_0));

//    ADC1->SQR3 &= ~(SQR3_SQ_Set << (5 * (1 - 1)));
//    ADC1->SQR3 |= (SQR3_SQ_Set << (5 * (1 - 1))) |
//                  (ADC_Channel_0 << (5 * (1 - 1)));

//    /* 使能ADC1 DMA功能 */
//    /* Enable the selected ADC DMA request */
//    ADC1->CR2 |= CR2_DMA_Reset;

//    /* 使能 ADC1 */
//    /* Set the ADON bit to wake up the ADC from power down mode */
//    ADC1->CR2 |= CR2_ADON_Set;

//    /* 使能ADC1 复位校准寄存器 */
//    /* Resets the selected ADC calibration registers */
//    ADC1->CR2 |= CR2_RSTCAL_Set;
//    /* 检查ADC1的复位寄存器 */
//    while ((ADC1->CR2 & CR2_RSTCAL_Set) != (uint32_t)RESET);

//    /* 启动ADC1校准 */
//    /* Enable the selected ADC calibration process */
//    ADC1->CR2 |= CR2_CAL_Set;
//    /* 检查校准是否结束 */
//    while ((ADC1->CR2 & CR2_CAL_Set) != (uint32_t)RESET);

//    /* 软件启动ADC转换 */
//    /* Enable the selected ADC conversion on external event and start the selected
//       ADC conversion */
//    ADC1->CR2 |= CR2_EXTTRIG_SWSTART_Set;
}

/*
*********************************************************************************************************
*	函 数 名: AdcPro
*	功能说明: ADC采样处理，插入1ms systick 中断进行调用
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AdcPro(void)
{
    static uint16_t buf[SAMP_COUNT];
    static uint8_t write;
    uint32_t sum;
    uint8_t i;

    buf[write] = ADC_GetConversionValue(ADC1);
    /* Return the selected ADC conversion value */
    // buf[write] = ADC1->DR;
    if (++write >= SAMP_COUNT)
    {
        write = 0;
    }

    /* 下面这段代码采用求平均值的方法进行滤波
        也可以改善下，选择去掉最大和最下2个值，使数据更加精确
    */
    sum = 0;
    for (i = 0; i < SAMP_COUNT; i++)
    {
        sum += buf[i];
    }
    g_usAdcValue = sum / SAMP_COUNT; /* ADC采样值由若干次采样值平均 */

     /* 软件启动下次ADC转换 */
    /* Enable the selected ADC conversion on external event and start the selected
      ADC conversion */
//    ADC1->CR2 |= CR2_EXTTRIG_SWSTART_Set;
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: GetADC
*	功能说明: 读取ADC采样的平均值
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t GetADC(void)
{
    uint16_t ret;

    /* 因为	g_AdcValue 变量在systick中断中改写，为了避免主程序读变量时被中断程序打乱导致数据错误，因此需要
    关闭中断进行保护 */

    /* 进行临界区保护，关闭中断 */
    __set_PRIMASK(1); /* 关中断 */

    ret = g_usAdcValue;

    __set_PRIMASK(0); /* 开中断 */

    return ret;
}
#endif
