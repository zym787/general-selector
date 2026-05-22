#define _MAIN_H_GLOBALS_
#include "common.h"
// ELAB_TAG("main"); /* elog 标签 */

// clang-format off

uint8_t valveFixDflt    = 0, 
        valveFixDir     = 0,
        valvePortCnt    = 10, 
        IntDflt         = 5, 
        SpdDflt         = INIT_SPD,
        bRdpDflt        = 0;

// clang-format on

void ParameterInit(void)
{
        uint8_t ReadBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        static uint8_t __bFirstInit = 0; /* 是否是第一次初始化标准 */

        /* 读取板号判断是否第一次进行初始化 */
        I2CPageRead_Nbytes(ADDR_BOARD_ID, LEN_BOARD_ID, ReadBuf);
        /* 读取默认参数 */
        if (BOARD_0 == ReadBuf[0] && BOARD_1 == ReadBuf[1]) {
                printd("\r 读取系统参数");

                printd("\r\n 基本参数");

                // 地址 0~63
                I2CPageRead_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &ags_mbParam.mAddrs);
                modbus.Address = ags_mbParam.mAddrs;
                printd("\r 地址: %d", ags_mbParam.mAddrs);

                /* 波特率 */
                I2CPageRead_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.baudrate);
                if (BAUD_MIN <= syspara.baudrate && BAUD_MAX >= syspara.baudrate) {
                        printd("\r 波特率: %d  %dbps", syspara.baudrate, BaudRate_V[syspara.baudrate]);
                } else {
                        syspara.baudrate = BAUD_9600;
                        printd("\r 波特率超限,默认写入%d 9600bps 请重新设置!", syspara.baudrate);
                        I2CPageWrite_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.baudrate);
                }

                // 通道数
                I2CPageRead_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
                if (!valveFix.fix.portCnt || valveFix.fix.portCnt > 32) {
                        valveFix.fix.portCnt = 10;
                }
                printd("\r 通道数: %d", valveFix.fix.portCnt);

                // 原点补偿
                I2CPageRead_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
                printd("\r 原点补偿: %d (0.1度)", Valve.fixOrg);

                // 方向补偿 无用
                I2CPageRead_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
                // printd("\r 方向补偿: %d (0.1度)", valveFix.fix.dirGap);

                I2CPageRead_Nbytes(ADDR_DIR_SD, LEN_DIR_SD, ReadBuf);
                Valve.fDirCw = ReadBuf[0];
                Valve.fDirCCw = ReadBuf[1];
                (!Valve.fDirCw || Valve.fDirCw > 100) ? (Valve.fDirCw = 1) : (Valve.fDirCw);
                (!Valve.fDirCCw || Valve.fDirCCw > 100) ? (Valve.fDirCCw = 1) : (Valve.fDirCCw);
                printd(
                    "\r\n 定位减速补偿: \
                \r\n 逆时针补偿CW:  %d (0.1度)\
                \r\n 顺时针补偿CCW: %d (0.1度)",
                    Valve.fDirCw, Valve.fDirCCw);

                // 烧机间隔
                I2CPageRead_Nbytes(ADDR_INTVL, LEN_INTVL, &syspara.agingInterval);
                printd("\r 老化间隔:%d 秒", syspara.agingInterval);

                /* 序列号 */
                I2CPageRead_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
                printd("\r\n 序列号:");
                for (uint8_t i = 0; i < 5; ++i)
                        printd(" %02X", *(Valve.SnCode + i));

                // 控制协议
                I2CPageRead_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &syspara.protocol_type);
                printd("\r\n 控制协议: %d %s", syspara.protocol_type,
                       (syspara.protocol_type) == AGS_MODBUS ? "AGS"
                       : (syspara.protocol_type) == EXT_COMM ? "EXTCOM_HX"
                       : (syspara.protocol_type) == MODBUS ? "MODBUS" : "wrong type");

                // 扫描标志
                I2CPageRead_Nbytes(ADDR_SYMBOL, LEN_SYMBOL, ReadBuf);
                sig.pulseBlock[0] = ReadBuf[0];
                sig.pulseBlock[0] <<= 8;
                sig.pulseBlock[0] |= ReadBuf[1];
                // lit blade 20 percent
                uint32_t temp = 0;
                temp = sig.pulseBlock[0] * PERCENT_TOLL;
                sig.pulseBlock[3] = temp / PERCENT;

                sig.pulseBlock[1] = ReadBuf[2];
                sig.pulseBlock[1] <<= 8;
                sig.pulseBlock[1] |= ReadBuf[3];
                // middle blade 8 percent
                temp = sig.pulseBlock[1] * PERCENT_TOLL;
                sig.pulseBlock[4] = temp / PERCENT;

                sig.pulseBlock[2] = ReadBuf[6];
                sig.pulseBlock[2] <<= 8;
                sig.pulseBlock[2] |= ReadBuf[7];
                // normal blade 20 percent
                temp = sig.pulseBlock[2] * PERCENT_TOLL;
                sig.pulseBlock[5] = temp / PERCENT;

                sig.pulseGap[0] = ReadBuf[4];
                sig.pulseGap[0] <<= 8;
                sig.pulseGap[0] |= ReadBuf[5];
                // mini Gap 8 percent
                temp = sig.pulseGap[0] * PERCENT_TOLL;
                sig.pulseGap[1] = temp / PERCENT;
                printd("\r\n 扫描宽度(±10%误差): ");
                printd("\r\n    主挡片 %d(%d)  次挡片 %d(%d)  小挡片 %d(%d)", sig.pulseBlock[0], sig.pulseBlock[3],
                       sig.pulseBlock[1], sig.pulseBlock[4], sig.pulseBlock[2], sig.pulseBlock[5]);
                printd("\r\n    小缺口 %d(%d)", sig.pulseGap[0], sig.pulseGap[1]);

                /* 减速比 */
                I2CPageRead_Nbytes(ADDR_RDC_RATE, LEN_RDC_RATE, &rdc.rate);
                switch (rdc.rate) {
                        case RDCR_1:
                                rdc.stepP1dgr = STEPS_1_DEGREE_RD01;
                                rdc.stepP01dgr = STEPS_01_DEGREE_RD01;
                                tBoundary.spd_max = 255;
                                break;
                        case RDCR_4:
                                rdc.stepP1dgr = STEPS_1_DEGREE_RD04;
                                rdc.stepP01dgr = STEPS_01_DEGREE_RD04;
                                tBoundary.spd_max = 255;
                                break;
                        case RDCR_10:
                                rdc.stepP1dgr = STEPS_1_DEGREE_RD10;
                                rdc.stepP01dgr = STEPS_01_DEGREE_RD10;
                                tBoundary.spd_max = 100;
                                break;
                        case RDCR_16:
                                rdc.stepP1dgr = STEPS_1_DEGREE_RD16;
                                rdc.stepP01dgr = STEPS_01_DEGREE_RD16;
                                tBoundary.spd_max = 70;
                                break;
                        case RDCR_20:
                                rdc.stepP1dgr = STEPS_1_DEGREE_RD16;
                                rdc.stepP01dgr = STEPS_01_DEGREE_RD16;
                                tBoundary.spd_max = 70;
                                break;
                        default:
                                printd("\r\n 减速比参数错误,缺省写入%d", RDCR_10);
                                rdc.rate = RDCR_10;
                                rdc.stepP1dgr = STEPS_1_DEGREE_RD10;
                                rdc.stepP01dgr = STEPS_01_DEGREE_RD10;
                                tBoundary.spd_max = 100;
                                break;
                }
                rdc.stepRound = P_ROUND;    // 单圈步数 200
                rdc.stepRound *= SCALE;     // 细分
                rdc.stepRound *= rdc.rate;  // 减速比
                printd("\r 减速比: %d  每一圈步数: %d", rdc.rate, rdc.stepRound);
                /* 半通道 */
                I2CPageRead_Nbytes(ADDR_HALF_SEAL, LEN_HALF_SEAL, &Valve.bHalfSeal);
                printd("\r 半通道: %d %s", Valve.bHalfSeal, (Valve.bHalfSeal) == 0 ? "关闭" : "开启");
#if ((defined IOCTRL) || (defined MUT_IOCTRL))
                I2CPageRead_Nbytes(ADDR_IO_CTRL, LEN_IO_CTRL, (uint8_t *)&syspara.ioCtrl);
                printd("\r\n IO控制: %d %s", syspara.ioCtrl, (syspara.ioCtrl) == false ? "关闭" : "开启");
#endif
                /// 停留时间
                I2CPageRead_Nbytes(ADDR_PAUSE_TIME, LEN_PAUSE_TIME, (uint8_t *)&syspara.pauseTime);
                printd("\r\n 中间状态停留时间: %d 毫秒", syspara.pauseTime);
                I2CPageRead_Nbytes(ADDR_BURN_CNT, LEN_BURN_CNT, (uint8_t *)&syspara.burnCnt);
                // printd("\r\n 老化次数 %d", syspara.burnCnt);
                /// 通道状态
#ifdef MUT_IOCTRL
                I2CPageRead_Nbytes(ADDR_STATE_CHANNEL, LEN_STATE_CHANNEL, Valve.StatusChannel);
                printd("\r\n           1  2  3  4");
                printd("\r\n 通道状态:");
                for (uint8_t i = 0; i < 4; ++i)
                        printd(" %d", *(Valve.StatusChannel + i));
#endif
                /* 切换次数 */
                I2CPageRead_Nbytes(ADDR_TOTAL_CNT, LEN_TOTAL_CNT, ((uint8_t *)&syspara.totalCnt));
                printd("\r\n 切换次数:%d", syspara.totalCnt);
                /* 模式 */
                I2CPageRead_Nbytes(ADDR_GOD_MODE, LEN_GOD_MODE, &syspara.GodMode);
                if (GD_NORMAL != syspara.GodMode) {
                        printd("\r 模式: %d %s", syspara.GodMode,
                               (syspara.GodMode) == GD_AGING     ? "老化模式"
                               : (syspara.GodMode) == GD_FACTORY ? "工厂模式"
                                                                 : "Normal模式");
                } else if (syspara.GodMode == GD_AGING) {
                        I2CPageRead_Nbytes(ADDR_BURN_CNT, LEN_BURN_CNT, (uint8_t *)&syspara.burnCnt);
                        printd("\r 老化次数: %d", syspara.burnCnt);
                        printd("\r 老化间隔: %d 秒", syspara.agingInterval);
                }
        } else {
                __bFirstInit = 1;
                Valve.bReInit = 0;
                /* 锁定驱动后再写入参数 */
                VALVE_ENA = DISABLE;
                printd("\r\n 写入默认参数");
                /* 板号 */
                ReadBuf[0] = 0x88;
                ReadBuf[1] = 0x66;
                I2CPageWrite_Nbytes(ADDR_BOARD_ID, LEN_BOARD_ID, ReadBuf);
                /* 地址 1 */
                ags_mbParam.mAddrs = AGS_ADDR_DEF;
                modbus.Address = ags_mbParam.mAddrs;
                I2CPageWrite_Nbytes(ADDR_MODULE_NUM, LEN_MODULE_NUM, &ags_mbParam.mAddrs);
                /* 波特率 1 9600bps */
                syspara.baudrate = BAUD_9600;
                I2CPageWrite_Nbytes(ADDR_BAUD, LEN_BAUD, &syspara.baudrate);
                /* 通道数 10 */
                valveFix.fix.portCnt = valvePortCnt;
#ifdef MUT_IOCTRL
                /// 多IO检测 12通定制
                valveFix.fix.portCnt = 12;
#endif
                I2CPageWrite_Nbytes(ADDR_PORT_CNT, LEN_PORT_CNT, &valveFix.fix.portCnt);
                /* 原点补偿 0 */
                Valve.fixOrg = valveFixDflt;
                I2CPageWrite_Nbytes(ADDR_VALVE_FIX, LEN_VALVE_FIX, &Valve.fixOrg);
                /* 方向补偿 0 */
                valveFix.fix.dirGap = valveFixDir;
                I2CPageWrite_Nbytes(ADDR_DIR_FIX, LEN_DIR_FIX, &valveFix.fix.dirGap);
                /* CW/CCW补偿 20 */
                Valve.fDirCw = 20;
                I2CPageWrite_Nbytes(ADDR_DIR_SD, LEN_DIR_SD - 1, &Valve.fDirCw);
                Valve.fDirCCw = 20;
                I2CPageWrite_Nbytes(ADDR_DIR_SD + 1, LEN_DIR_SD - 1, &Valve.fDirCCw);
                /* 老化间隔 5秒 */
                syspara.agingInterval = IntDflt;
                I2CPageWrite_Nbytes(ADDR_INTVL, LEN_INTVL, &syspara.agingInterval);
                /* 减速比 10 */
                rdc.rate = RDCR_10;
                I2CPageWrite_Nbytes(ADDR_RDC_RATE, LEN_RDC_RATE, &rdc.rate);
                /* 速度 */
                Valve.spd = INIT_SPD;
                I2CPageWrite_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
                /* 半通道 0 */
                Valve.bHalfSeal = 0;
                I2CPageWrite_Nbytes(ADDR_HALF_SEAL, LEN_HALF_SEAL, &Valve.bHalfSeal);
                /* 序列号 */
                memset(Valve.SnCode, 0, sizeof(Valve.SnCode));
                I2CPageWrite_Nbytes(ADDR_SN, LEN_SN, Valve.SnCode);
                /* 协议 */
                syspara.protocol_type = AGS_MODBUS;
                I2CPageWrite_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &syspara.protocol_type);
                /// 停留时间
                syspara.pauseTime = 0;
                I2CPageWrite_Nbytes(ADDR_PAUSE_TIME, LEN_PAUSE_TIME, (uint8_t *)&syspara.pauseTime);
                /// IO控制
                syspara.ioCtrl = true;
                I2CPageWrite_Nbytes(ADDR_IO_CTRL, LEN_IO_CTRL, (uint8_t *)&syspara.ioCtrl);
                /// 老化次数
                syspara.burnCnt = 0;
                I2CPageWrite_Nbytes(ADDR_BURN_CNT, LEN_BURN_CNT, (uint8_t *)&syspara.burnCnt);
                syspara.GodMode = GD_NORMAL;
                I2CPageWrite_Nbytes(ADDR_GOD_MODE, LEN_GOD_MODE, &syspara.GodMode);
                /// 通道状态
#ifdef MUT_IOCTRL
                uint8_t temp[4] = {1, 2, 3, 4};
                memcpy(Valve.StatusChannel, temp, 4);
                I2CPageWrite_Nbytes(ADDR_STATE_CHANNEL, LEN_STATE_CHANNEL, Valve.StatusChannel);
#endif

                // 写入参数后 锁定驱动
                VALVE_ENA = DISABLE;
                printd("\r 写入成功,请复位!!!");
        }
#ifdef MUT_IOCTRL
        /// 范围检查
        for (uint8_t i = 0; i < 4; i++) {
                if (1 > Valve.StatusChannel[i] || Valve.StatusChannel[i] > valveFix.fix.portCnt) {
                        printd("\r\n 错误! 第%d个通道值%d超限 写入默认值 1 2 3 4", i + 1, Valve.StatusChannel[i]);
                        uint8_t temp[4] = {1, 2, 3, 4};
                        memcpy(Valve.StatusChannel, temp, 4);
                        I2CPageWrite_Nbytes(ADDR_STATE_CHANNEL, LEN_STATE_CHANNEL, Valve.StatusChannel);
                        break;
                }
        }
#endif
        getOptStartStatus();
        /* 设置速度范围 */
        tBoundary.spd_min = SPD_MIN;
        tBoundary.spd_init = INIT_SPD; /* 初始化速度默认情况为15RPM */
        /* 速度 */
        I2CPageRead_Nbytes(ADDR_SPD, LEN_SPD, &Valve.spd);
        if (tBoundary.spd_min > Valve.spd || tBoundary.spd_max < Valve.spd)
                Valve.spd = tBoundary.spd_min;
        printd("\r 速度: %d RPM", Valve.spd);

        /* 使用初始化速度找原点 */
        speed[AXSV] = 100;
        accel[AXSV] = 100;
        decel[AXSV] = 200;
        speed[AXSV] *= (tBoundary.spd_init);
        speed[AXSV] *= (rdc.rate);
        accel[AXSV] *= (tBoundary.spd_init);
        accel[AXSV] *= (rdc.rate);
        decel[AXSV] *= (tBoundary.spd_init);
        decel[AXSV] *= (rdc.rate);
        dbg_printf("\r\n 初始化运动 减速!  (%dRPM) spd%d acc%d dec%d", tBoundary.spd_init, speed[AXSV], accel[AXSV],
                   decel[AXSV]);
        VALVE_ENA = ENABLE;
        Valve.status = VALVE_INITING;
        Valve.ErrBlinkTime = NORMAL_BLINK;
        Valve.passByOne = 0;
        if (0 == __bFirstInit) {
                Valve.bReInit = 1;
        }
        Valve.bNewInit = 0xff;
        Valve.goFirstFlag = 0;
}

/*
    GPIO初始化
*/
void GPIOInit(void)
{
        /// IO初始化
#if ((defined IOCTRL) || (defined MUT_IOCTRL))
        bsp_IOInit();
        /// ADC初始化
#ifdef MUT_IOCTRL
        ADC_Configuration();
#endif
#endif

        /* PB1 为485芯片收发切换引脚 */
        RCC->APB2ENR |= (RCC_APB2Periph_GPIOB);
        GPIOB->CRL &= (GPIO_Crl_P1);
        GPIOB->CRL |= (GPIO_Mode_Out_PP_50MHz_P1);
        RX_EN();  // 开机为接收模式
}

void ErrBlink(void)
{
        /* 设置led闪烁间隔 */
        if (timerPara.timeOut > Valve.ErrBlinkTime) {
                timerPara.timeOut = 0;
                LED_WORK = !LED_WORK;
        }
}

// 出错响应立即停机
uint8_t errActionImme(void)
{
        if (Valve.status != VALVE_ERR) {
                srd[AXSV].accel_count = -1;
                srd[AXSV].run_state = DECEL;
                Valve.portDes = 0;
                Valve.ErrBlinkTime = RETRY_TIME_OUT;
                Valve.status = VALVE_ERR;
                VALVE_ENA = DISABLE;
                return 1;
        }
        return 0;
}

#if 0
void errProcRun(void)
{
    if (!(Valve.status & VALVE_INITING))
    {
        // 复位时不做重新找位动作
        srd[AXSV].accel_count = -1;
        srd[AXSV].run_state = DECEL;
        if (Valve.retryTms < RETRY_TIMES)
        {
            VALVE_ENA = ENABLE;
            Valve.status = VALVE_INITING;
            Valve.ErrBlinkTime = NORMAL_BLINK;
            Valve.passByOne = 0;
            Valve.bReInit = 1;
            Valve.bNewInit = 1;
            Valve.portCur = 0;
            Valve.initStep = 0;
            syspara.protectTimeOut = 0;
            printd("\r\n %d重试 ->%d", Valve.retryTms, Valve.portDes);
        }
        else
        {
            if (errActionImme())
                printd("\r\n 重试超时!");
        }
    }
    else
    {
        if (errActionImme())
            printd("\r\n 初始化错误");
    }
}
#endif

// 检测任务
// 1 半通道
// 2 超时检测任务(1s)
#define SINGLE_RUN_TIMEOUT    5   // 运行5秒超时
#define SINGLE_INITING_TIMOUT 14  // 转一圈差不多3秒，复位单次是两圈
void everySecDo(void)
{
        /* 开机1号孔 */
#ifdef FIRST_HOLE
        if (!Valve.bHalfSeal) {
                // 非半通道走位
                if (!(Valve.status & VALVE_INITING) && Valve.bNewInit == 1) {
                        Valve.dir = CCW;
                        Valve.portDes = 1; /* C 开机1号通 */
                        Valve.dir = 0xFF;  /* 就近 */
                        dbg_printf("\r\n %d->%d Dir:%d", Valve.portCur, Valve.portDes, Valve.dir);
                        Valve.bNewInit = 0;
                        Valve.goFirstFlag = 1;
                        printd("\r\n 到1号位  当前位置:%d", Valve.portCur);
                }
        }
#endif  // FIRST_HOLE

        /// E版本 带IO
#ifdef IOCTRL
        /// IO检测
        if (2 == Valve.goFirstFlag) {
                if (DCSEC < timerPara.timeMilli) {
                        timerPara.timeMilli = 0;
                        bsp_IODetect();
                }
        }
#endif  // IOCTRL

        /// F版本 多IO
#ifdef MUT_IOCTRL
        /// 多IO检测
        static uint8_t times = 0;
        if (mSEC < timerPara.timeMilli) {
                timerPara.timeMilli = 0;
                ++times;
                // AdcPro();
                if (0 == times % 21) {
                        times = 0;
                        bsp_IODetect();
                }
        }
#endif

        // 每秒检测一次
        if (timerPara.sec > SEC) {
                timerPara.sec = 0;

                /* 切换次数发生变化时写入 */
                if (VALVE_RUN_END == Valve.status) {
                        if (syspara.totalCnt != syspara.totalCntLst) {
                                syspara.totalCntLst = syspara.totalCnt;
                                I2CPageWrite_Nbytes(ADDR_TOTAL_CNT, LEN_TOTAL_CNT, (uint8_t *)&syspara.totalCnt);
                        }
                }

                /* 超时报错 */
                // 单通道间做5秒的超时处理，避免长时间堵转烧坏电路
                if ((Valve.status == VALVE_RUNNING && syspara.protectTimeOut > SINGLE_RUN_TIMEOUT * SEC) ||
                    (Valve.status & VALVE_INITING && syspara.protectTimeOut > SINGLE_INITING_TIMOUT * SEC)) {
                        if (!(Valve.status & VALVE_ERR)) {
                                Valve.portDes = 0;
                                Valve.status = VALVE_ERR;
                                VALVE_ENA = DISABLE;
                        } else {
                                VALVE_ENA = DISABLE;
                        }
                        printd("\r\n 运行超时! (initstep%d,%dms)", Valve.initStep, syspara.protectTimeOut);
                        Valve.ErrBlinkTime = RETRY_TIME_OUT;
                }
                // 15秒超时锁机
                if (syspara.protectTimeOut > (SINGLE_INITING_TIMOUT + 1) * SEC) {
                        Valve.status = VALVE_ERR;
                        VALVE_ENA = DISABLE;
                        printd("\r\n %d 超时保护! (initstep%d,%dms)", SINGLE_INITING_TIMOUT + 1, Valve.initStep,
                               syspara.protectTimeOut);
                        Valve.ErrBlinkTime = RETRY_TIME_OUT;
                }
        }
}

/*

*/
int main(void)
{
        Stm32_Clock_Init(9); /* 系统时钟设置 */
        delay_init(72);      /* 延时初始化 */
#if 0
    JTAG_Set(JTAG_SWD_DISABLE);
#else
    JTAG_Set(SWD_ENABLE);
#endif
        delay_ms(100);
        Usart1_Init(72, 115200); /* 串口初始化为115200 */
        iic_INIT();
        ConfigValve();
        TIM2_Init(999, 71);   /* 10Khz的计数频率 */
        TIM4_Init(65535, 35); /* X轴脉冲定时器 */
        GPIOInit();
        delay_ms(100);
        BootInterface();
        printd(
            "\r\n Version:%s(%08X)  Time: %s %s \
            \r\n Description:%s  %s  (%s) %s\
            \r\n PCB:%s  %s \r\n",
            SOFT_VER_C, SOFT_VER, __DATE__, __TIME__, DESCRIPTION, HOLE_INFO, CONTROL, LTS, PCB_VR,
            HARDWARE_DESCRIPTION);
#ifdef FIRST_HOLE_MUT_IO_F
        I2CPageRead_Nbytes(ADDR_STATE_CHANNEL, LEN_STATE_CHANNEL, Valve.StatusChannel);
        delay_ms(100);
        printd("\r\n-------------------多IO控制说明 (低电平有效)-------------------");
        printd("\r\n 输入:    IN1     IN2   | OUT1   OUT2   通道 (状态)");
        printd("\r\n         1/悬空  1/悬空 |  1      1      %d   (1)", Valve.StatusChannel[0]);
        printd("\r\n           0     1/悬空 |  0      1      %d   (2)", Valve.StatusChannel[1]);
        printd("\r\n         1/悬空    0    |  1      0      %d   (3)", Valve.StatusChannel[2]);
        printd("\r\n           0       0    |  0      0      %d   (4)", Valve.StatusChannel[3]);
        printd("\r\n 运行中: FBOUT输出1,OUT1/OUT2保持先前状态,到位后FBOUT输出0");
        printd("\r\n 报错时: 无论IN1/IN2输入何值,ERROUT输出0,FBOUT输出1,OUT1/OUT2保持先前状态");
        printd("\r\n-------------------------------------------------------------\r\n");
#endif
        I2CPageRead_Nbytes(ADDR_PROTOCAL, LEN_PROTOCAL, &syspara.protocol_type);
        /* 根据协议初始化 */
        if (syspara.protocol_type == AGS_MODBUS) {
                ags_mbInit(); /* AGS协议 */
        } else if (syspara.protocol_type == EXT_COMM) {
                CommInit(); /* HX协议 */
        } else if (syspara.protocol_type == MODBUS) {
                mb_Init(); /* 初始化Modbus协议 */
        }
        ParameterInit();
        while (1) {
                /* 协议栈轮询 */
                if (syspara.protocol_type == AGS_MODBUS) {
                        ags_mbProcess(); /* AGS协议 */
                } else if (syspara.protocol_type == EXT_COMM) {
                        UsartProcess(); /* HX协议 */
                } else if (syspara.protocol_type == MODBUS) {
                        mb_Poll(); /* 解析Modbus数据帧 */
                }
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
        if (Valve.bPassPort) {
                Valve.bPassPort = 0;
                printd("\r\n P %d  B %d", Valve.portCur, syspara.OptBlockLast);
        }
        if (timerPara.timeDbg > SEC * 5) {
                timerPara.timeDbg = 0;
                // LED_WORK = !LED_WORK;
                //        dbg_printf("\r\n >>重试:%d,OptBlock:%d,Opt:%d,bNewInit:%d",
                //                Valve.retryTms, Valve.OptBlock, VALVE_OPT, Valve.bNewInit);
                dbg_printf("\r\n >>OptBlock:%d,Opt:%d,bNewInit:%d", Valve.OptBlock, VALVE_OPT, Valve.bNewInit);
                dbg_printf("\r\n   状态:0x%02x,当前位:%d,目标位:%d,方向:%d", Valve.status, Valve.portCur, Valve.portDes,
                           srd[0].dir);
                if (syspara.protocol_type == AGS_MODBUS) {
                        dbg_printf("  AGS %d %d", ags_mbParam.rCnt, ags_mbParam.times);
                } else if (syspara.protocol_type == EXT_COMM) {
                        dbg_printf("  EXTCOM %d %d", protext.stepCnt, protext.time);
                } else if (syspara.protocol_type == MODBUS) {
                        dbg_printf("  Modbus %d %d", modbus.ReciveCount, modbus.times);
                }
#ifdef IOCTRL
                dbg_printf("\r\n   IO_IN:%d IO_OUT:%d  停留时间:%d  Ctrl:%d", IO_IN, IO_OUT, syspara.pauseTime,
                           syspara.ctrlPause);
#endif
#ifdef MUT_IOCTRL
                dbg_printf("\r\n   IN1:%d IN2:%d  OUT1:%d OUT2:%d  FBOUT:%d ERROUT:%d", IO_IN1, IO_IN2, IO_OUT1,
                           IO_OUT2, IO_FBOUT, IO_ERROUT);
#if 0
        /* 注意： 末尾只有 \r回车, 没有\n换行，可以使PC超级终端界面稳定在1行显示 */
        {
            ///Get ADC
            uint16_t adc = GetADC();
            /* 超级终端界面上会显示一个不断旋转的字符
            增加这个功能，是为了避免程序死机的假象，因为ADC采样值很稳定
            */
            static uint8_t pos = 0;

            if (pos == 0)
                dbg_printf("\r\n   |");
            else if (pos == 1)
                dbg_printf("\r\n   /");
            else if (pos == 2)
                dbg_printf("\r\n   -");
            else if (pos == 3)
                dbg_printf("\r\n   \\");		/* 注意：这个特殊字符需要转义 */

            if (++pos >= 4)
            {
                pos = 0;
            }

            dbg_printf("PA0口线ADC1_CH0采样值=%5d  电压=%4dmV\r",
                adc, ((uint32_t)adc * 3300) / 4095);
        }
#endif
#endif
        }
}
