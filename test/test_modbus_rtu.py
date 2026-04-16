#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@file      : test_modbus_rtu.py
@brief     : Modbus RTU 主机测试脚本 - HIL测试
@author    : Drinkto
@date      : 2026-04-15

使用 pymodbus 库对 general-selector 设备进行 Modbus RTU 通信测试。
支持功能码: 03H(读保持寄存器), 06H(写单个保持寄存器)

依赖安装:
    pip install pymodbus pyserial

使用方法:
    python test_modbus_rtu.py --port COM4 --baudrate 9600 --unit 1
    python test_modbus_rtu.py --port COM4 --skip-motion   # 跳过阀体运动测试
"""

import argparse
import sys
import time
from enum import IntEnum

try:
    from pymodbus.client import ModbusSerialClient
except ImportError:
    print("错误: 请先安装 pymodbus 库")
    print("  pip install pymodbus pyserial")
    sys.exit(1)


# ===== 寄存器地址定义 (与 modbus.h 保持一致) =====

class CtrlReg(IntEnum):
    """控制指令寄存器 CTRL (0-9)"""
    SET_NORMAL   = 0
    SET_CW       = 1
    SET_CCW      = 2
    SET_FREE     = 3
    REG_4        = 4
    SET_ZERO     = 5
    REG_6        = 6
    REG_7        = 7
    REG_8        = 8
    SET_GOD_MODE = 9


class StatusReg(IntEnum):
    """只读状态寄存器 STATUS (10-19)"""
    CHANNEL_CUR    = 10
    CONTROL_STATE  = 11
    MOVE_TIME      = 12
    SW_CODE        = 13
    SW_VERSION     = 14
    COUNT_1        = 15
    COUNT_2        = 16
    REG_17         = 17
    REG_18         = 18
    REG_19         = 19


class Operate1Reg(IntEnum):
    """运行参数1寄存器 OPERATE1 (20-29)"""
    ADDRESS      = 20
    SPEED        = 21
    DIRECTION    = 22
    BAUDRATE     = 23
    MOVE_COUNT_1 = 24
    MOVE_COUNT_2 = 25
    REG_26       = 26
    REG_27       = 27
    REG_28       = 28
    REG_29       = 29


class UserReg(IntEnum):
    """序列号寄存器 USER (40-49)"""
    SN_1  = 40
    SN_2  = 41
    SN_3  = 42
    SN_4  = 43
    SN_5  = 44
    SN_6  = 45
    SN_7  = 46
    SN_8  = 47
    SN_9  = 48
    SN_10 = 49


class Factory1Reg(IntEnum):
    """出厂参数1 FACTORY1 (50-59)"""
    UID_X0   = 50
    UID_X1   = 51
    UID_Y0   = 52
    UID_Y1   = 53
    UID_Z0   = 54
    UID_Z1   = 55
    DATA_01  = 56
    DATA_02  = 57
    DATA_03  = 58
    DATA_04  = 59


class Factory2Reg(IntEnum):
    """出厂参数2 FACTORY2 (60-69)"""
    VALVE_TYPE  = 60
    CTRL_MODE   = 61
    CHANNEL_NUM = 62
    HALF_MODE   = 63
    REPLY_MODE  = 64
    SECURE_CODE = 65
    COMPEN_ORG  = 66
    COMPEN_DIR  = 67
    COMPEN_CW   = 68
    COMPEN_CCW  = 69


# 安全码定义
NORMAL_CODE   = 0x22
AGING_CODE    = 0xEA
SECURITY_CODE = 0xFC

# MODBUS_NUMBER
MODBUS_NUMBER = 100


# ===== 测试结果统计 =====

class TestResult:
    def __init__(self):
        self.passed = 0
        self.failed = 0
        self.skipped = 0
        self.errors = []

    def ok(self, name):
        self.passed += 1
        print(f"  [PASS] {name}")

    def fail(self, name, detail=""):
        self.failed += 1
        msg = f"  [FAIL] {name}"
        if detail:
            msg += f" - {detail}"
        print(msg)
        self.errors.append((name, detail))

    def skip(self, name, reason=""):
        self.skipped += 1
        msg = f"  [SKIP] {name}"
        if reason:
            msg += f" - {reason}"
        print(msg)

    def summary(self):
        total = self.passed + self.failed + self.skipped
        print(f"\n{'='*50}")
        print(f"测试总数: {total}  通过: {self.passed}  失败: {self.failed}  跳过: {self.skipped}")
        if self.errors:
            print(f"\n失败详情:")
            for name, detail in self.errors:
                print(f"  - {name}: {detail}")
        print(f"{'='*50}")
        return self.failed == 0


# ===== 辅助函数 =====

def read_holding(client, unit, address, count=1):
    """读取保持寄存器,返回寄存器值列表或None"""
    try:
        result = client.read_holding_registers(address=address, count=count, device_id=unit)
        time.sleep(0.5)  # 请求间延时,避免RS485帧堆积
        if result.isError():
            return None
        return result.registers
    except Exception as e:
        time.sleep(1.0)  # 异常后等更久
        print(f"    [通信异常] 读寄存器{address}: {e}")
        return None


def write_holding(client, unit, address, value):
    """写单个保持寄存器,返回True/False"""
    try:
        result = client.write_register(address=address, value=value, device_id=unit)
        time.sleep(0.5)  # 请求间延时
        return not result.isError()
    except Exception as e:
        time.sleep(1.0)
        print(f"    [通信异常] 写寄存器{address}: {e}")
        return False


# ===== 测试用例 =====

def test_connection(client, unit, result: TestResult):
    """基本连接和通信"""
    name = "基本通信连接"
    regs = read_holding(client, unit, StatusReg.CHANNEL_CUR, 1)
    if regs is not None:
        result.ok(name)
    else:
        result.fail(name, "无法读取设备寄存器,请检查串口和地址")


def test_read_status_registers(client, unit, result: TestResult):
    """读取状态寄存器(10-19)"""
    name = "读状态寄存器(10-19)"
    regs = read_holding(client, unit, StatusReg.CHANNEL_CUR, 10)
    if regs is None:
        result.fail(name, "读取失败")
        return

    ch_cur = regs[StatusReg.CHANNEL_CUR - StatusReg.CHANNEL_CUR]
    ctrl_state = regs[StatusReg.CONTROL_STATE - StatusReg.CHANNEL_CUR]
    print(f"    当前通道: {ch_cur}, 控制状态: {ctrl_state}")
    result.ok(name)


def test_read_current_channel(client, unit, result: TestResult):
    """读取当前通道"""
    name = "读当前通道"
    regs = read_holding(client, unit, StatusReg.CHANNEL_CUR, 1)
    if regs is None:
        result.fail(name, "读取失败")
        return

    ch = regs[0]
    if ch == 0xFF:
        print(f"    当前通道: 0xFF (阀体未初始化)")
        result.ok(name)
    elif 0 <= ch <= 32:
        result.ok(name)
    else:
        result.fail(name, f"通道值异常: {ch} (预期0-32或0xFF未初始化)")


def test_read_software_version(client, unit, result: TestResult):
    """读取软件版本"""
    name = "读软件版本"
    regs = read_holding(client, unit, StatusReg.SW_CODE, 2)
    if regs is None:
        result.fail(name, "读取失败")
        return

    print(f"    软件代号: {regs[0]}, 版本: {regs[1]}")
    result.ok(name)


def test_read_operate1(client, unit, result: TestResult):
    """读取运行参数1"""
    name = "读运行参数1(20-25)"
    regs = read_holding(client, unit, Operate1Reg.ADDRESS, 6)
    if regs is None:
        result.fail(name, "读取失败")
        return

    addr = regs[Operate1Reg.ADDRESS - Operate1Reg.ADDRESS]
    spd = regs[Operate1Reg.SPEED - Operate1Reg.ADDRESS]
    baud = regs[Operate1Reg.BAUDRATE - Operate1Reg.ADDRESS]
    print(f"    地址: {addr}, 速度: {spd}, 波特率编码: {baud}")
    result.ok(name)


def test_read_uid(client, unit, result: TestResult):
    """读取芯片UID"""
    name = "读芯片UID(50-55)"
    regs = read_holding(client, unit, Factory1Reg.UID_X0, 6)
    if regs is None:
        result.fail(name, "读取失败")
        return

    uid = (regs[0] << 16 | regs[1], regs[2] << 16 | regs[3], regs[4] << 16 | regs[5])
    print(f"    UID: {uid[0]:08X}-{uid[1]:08X}-{uid[2]:08X}")
    result.ok(name)


def test_write_read_channel(client, unit, result: TestResult):
    """写入通道指令并验证状态变化"""
    name = "写通道指令+读状态验证"

    regs_before = read_holding(client, unit, StatusReg.CHANNEL_CUR, 1)
    if regs_before is None:
        result.fail(name, "读取当前通道失败")
        return

    ch_before = regs_before[0]
    target_ch = 1 if ch_before != 1 else 2

    ok = write_holding(client, unit, CtrlReg.SET_NORMAL, target_ch)
    if not ok:
        result.fail(name, "写入通道指令失败")
        return

    time.sleep(5)

    regs_after = read_holding(client, unit, StatusReg.CHANNEL_CUR, 1)
    if regs_after is None:
        result.fail(name, "读回通道失败")
        return

    ch_after = regs_after[0]
    if ch_after == target_ch:
        result.ok(name)
    else:
        result.fail(name, f"通道未切换: 期望{target_ch}, 实际{ch_after}")


def test_reset_command(client, unit, result: TestResult):
    """复位指令"""
    name = "复位指令(CTRL_SET_ZERO)"

    ok = write_holding(client, unit, CtrlReg.SET_ZERO, 1)
    if ok:
        time.sleep(5)
        regs = read_holding(client, unit, StatusReg.CONTROL_STATE, 1)
        if regs is not None:
            print(f"    复位后状态: {regs[0]}")
            result.ok(name)
        else:
            result.fail(name, "复位后读取状态失败")
    else:
        result.fail(name, "写入复位指令失败")


def test_invalid_register_address(client, unit, result: TestResult):
    """读取无效寄存器地址(>=MODBUS_NUMBER)"""
    name = "无效寄存器地址(>=100)"

    regs = read_holding(client, unit, MODBUS_NUMBER, 1)
    if regs is None:
        result.ok(name)
    else:
        result.fail(name, f"超出范围的地址不应返回数据: {regs}")


def test_read_too_many_registers(client, unit, result: TestResult):
    """请求读取过多寄存器"""
    name = "请求过多寄存器(从90读20个)"

    regs = read_holding(client, unit, 90, 20)
    if regs is None:
        result.ok(name)
    else:
        result.fail(name, "超出范围的读取应返回错误")


def test_write_read_speed(client, unit, result: TestResult):
    """读写速度参数"""
    name = "读写速度参数"

    regs = read_holding(client, unit, Operate1Reg.SPEED, 1)
    if regs is None:
        result.fail(name, "读取速度失败")
        return

    original_speed = regs[0]
    test_speed = 50 if original_speed != 50 else 80

    ok = write_holding(client, unit, Operate1Reg.SPEED, test_speed)
    if not ok:
        result.fail(name, "写入速度失败")
        return

    regs = read_holding(client, unit, Operate1Reg.SPEED, 1)
    if regs is not None and regs[0] == test_speed:
        result.ok(name)
    else:
        result.fail(name, f"速度不匹配: 写入{test_speed}, 读回{regs}")

    write_holding(client, unit, Operate1Reg.SPEED, original_speed)


def test_read_all_register_groups(client, unit, result: TestResult):
    """批量读取所有寄存器分组"""
    groups = [
        ("CTRL(0-9)",       0,  10),
        ("STATUS(10-19)",   10, 10),
        ("OPERATE1(20-29)", 20, 10),
        ("OPERATE2(30-39)", 30, 10),
        ("USER(40-49)",     40, 10),
        ("FACTORY1(50-59)", 50, 10),
        ("FACTORY2(60-69)", 60, 10),
        ("BACKUP1(80-89)",  80, 10),
        ("BACKUP2(90-99)",  90, 10),
    ]

    all_ok = True
    for group_name, addr, count in groups:
        regs = read_holding(client, unit, addr, count)
        if regs is None:
            result.fail(f"读寄存器组{group_name}", "读取失败")
            all_ok = False
        else:
            print(f"    {group_name}: {[f'{r:04X}' for r in regs[:4]]}...")

    if all_ok:
        result.ok("批量读所有寄存器组")


def test_god_mode_normal(client, unit, result: TestResult):
    """设置正常模式"""
    name = "设置正常模式(NORMAL_CODE=0x22)"

    ok = write_holding(client, unit, CtrlReg.SET_GOD_MODE, NORMAL_CODE)
    if ok:
        result.ok(name)
    else:
        result.fail(name, "写入失败")


def test_write_address(client, unit, result: TestResult):
    """修改从站地址(谨慎)"""
    name = "修改从站地址"
    result.skip(name, "修改地址可能失联,需手动测试")


def test_broadcast_address(client, unit, result: TestResult):
    """广播地址写操作"""
    name = "广播地址写操作"
    result.skip(name, "广播地址无响应,需通过逻辑分析仪验证")


# ===== 主函数 =====

def main():
    parser = argparse.ArgumentParser(description="General-Selector Modbus RTU 测试脚本")
    parser.add_argument("--port", default="COM4", help="串口设备 (默认: COM4)")
    parser.add_argument("--baudrate", type=int, default=9600, choices=[9600, 19200, 38400],
                        help="波特率 (默认: 9600)")
    parser.add_argument("--unit", type=int, default=1, help="从站地址 (默认: 1)")
    parser.add_argument("--parity", default="N", choices=["N", "E", "O"], help="校验位 (默认: N)")
    parser.add_argument("--stopbits", type=int, default=1, help="停止位 (默认: 1)")
    parser.add_argument("--timeout", type=float, default=1.0, help="响应超时秒 (默认: 1.0)")
    parser.add_argument("--skip-motion", action="store_true", help="跳过涉及阀体运动的测试")
    args = parser.parse_args()

    print(f"{'='*50}")
    print(f"General-Selector Modbus RTU HIL 测试")
    print(f"串口: {args.port}  波特率: {args.baudrate}  从站: {args.unit}")
    print(f"{'='*50}\n")

    client = ModbusSerialClient(
        port=args.port,
        baudrate=args.baudrate,
        parity=args.parity,
        stopbits=args.stopbits,
        bytesize=8,
        timeout=args.timeout,
        retries=0,       # 不重试,避免RS485帧堆积
    )

    if not client.connect():
        print(f"错误: 无法打开串口 {args.port}")
        sys.exit(1)

    print(f"串口已连接: {args.port}\n")

    result = TestResult()

    # 基础通信测试
    print("--- 基础通信测试 ---")
    test_connection(client, args.unit, result)
    test_read_status_registers(client, args.unit, result)
    test_read_current_channel(client, args.unit, result)
    test_read_software_version(client, args.unit, result)

    # 寄存器读取测试
    print("\n--- 寄存器读取测试 ---")
    test_read_operate1(client, args.unit, result)
    test_read_uid(client, args.unit, result)
    test_read_all_register_groups(client, args.unit, result)

    # 写入控制测试
    print("\n--- 写入控制测试 ---")
    if not args.skip_motion:
        test_write_read_channel(client, args.unit, result)
        test_reset_command(client, args.unit, result)
    else:
        result.skip("写通道+验证", "用户指定跳过运动测试")
        result.skip("复位指令", "用户指定跳过运动测试")

    test_write_read_speed(client, args.unit, result)
    test_god_mode_normal(client, args.unit, result)

    # 边界与异常测试
    print("\n--- 边界与异常测试 ---")
    test_invalid_register_address(client, args.unit, result)
    test_read_too_many_registers(client, args.unit, result)

    # 高级功能
    print("\n--- 高级功能测试 ---")
    test_write_address(client, args.unit, result)
    test_broadcast_address(client, args.unit, result)

    client.close()

    success = result.summary()
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
