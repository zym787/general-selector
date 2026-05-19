import os
import subprocess
import shutil
import argparse
import glob
import zipfile
import sys
import datetime
import logging
import re

# ========================
# 1. 配置区域 (只需修改这里)
# ========================

# 项目配置定义所有TARGETS 对应Keil不同的TARGET
# 开机1号孔 C版本
# 开机末端孔 D版本
# 开机1号孔带IO控制 E版本
# 开机1号孔多IO控制 F版本
TARGETS = [
    "C_1FH",
    "D_EH",
    "E_1IO",
    "F_1MIO",
]
OBJ_DIR = "OBJ"  # 编译产物目录
SOFTWARE_NAME = f"XZF_v2.0.0"  # 软件名称

# 默认路径参数  注意路径!!!必须用\反斜杠
DEFAULT_KEIL_PATH = "D:\\toolchain\\Keil_v5\\UV4\\UV4.exe"
DEFAULT_PROJECT_PATH = ".\\USER\\XZF.uvprojx"
DEFAULT_VERSION_PATH = ".\\Version"
DEFAULT_OUTPUT_PATH = ".\\Version\\output"

# 目标名称到Version文件夹的映射关系
TARGET_VERSION_MAP = {
    "C_1FH": ["C", "909开机1号孔", "(开启LTS)"],
    "D_EH": ["D", "909开机末端孔", "(开启LTS)"],
    "E_1IO": ["E", "909开机1号孔带IO控制", "(开启LTS)"],
    "F_1MIO": ["F", "909开机1号孔带多IO控制", "(开启LTS)"],
}

# 确保控制台输入支持中文（Windows系统需要）
if os.name == "nt":
    import io

    sys.stdin = io.TextIOWrapper(sys.stdin.buffer, encoding="utf-8")
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8")


# ========================
# 2. 统一日志接口 (核心优化)
# ========================
def init_logger(log_file_path):
    """初始化日志系统，输出到文件和控制台"""
    logger = logging.getLogger("release_logger")
    logger.setLevel(logging.INFO)

    # 控制台处理器
    console_handler = logging.StreamHandler()
    console_handler.setFormatter(logging.Formatter("%(message)s"))
    logger.addHandler(console_handler)

    # 文件处理器 (使用覆盖模式,每次运行清空旧日志)
    file_handler = logging.FileHandler(
        log_file_path, mode="w", encoding="utf-8")
    file_handler.setFormatter(logging.Formatter("%(message)s"))
    logger.addHandler(file_handler)

    return logger


def log_info(message):
    """统一日志接口：记录信息并刷新控制台"""
    logger = logging.getLogger("release_logger")
    logger.info(message)
    sys.stdout.flush()


# ========================
# 3. 工具函数模块 (使用统一日志接口)
# ========================
def clear_obj_folder(obj_dir):
    """清空OBJ文件夹内的所有文件（保留文件夹本身）"""
    if not os.path.exists(obj_dir):
        log_info(f"⚠️警告: OBJ目录不存在 ({obj_dir})，将创建它")
        os.makedirs(obj_dir, exist_ok=True)
        return

    for file in os.listdir(obj_dir):
        file_path = os.path.join(obj_dir, file)
        try:
            if os.path.isfile(file_path) or os.path.islink(file_path):
                os.unlink(file_path)
            elif os.path.isdir(file_path):
                shutil.rmtree(file_path)
        except Exception as e:
            log_info(f"⚠️警告: 无法删除 {file_path} - {str(e)}")

    log_info(f"✅ 已清空 OBJ 文件夹: {obj_dir}")


def after_run_clear():
    for file in os.listdir("."):
        if file == f"XZF.hex" or file == f"keil_compile_log.txt":
            os.remove(file)
            log_info(f"🗑️ 已删除: {file}")


def get_hex_file(OBJ_DIR):
    """在OBJ目录中查找第一个.hex文件"""
    for file in os.listdir(OBJ_DIR):
        if file.endswith(".hex"):
            return os.path.join(OBJ_DIR, file)
    log_info(f"❌ 错误: OBJ目录中未找到.hex文件: {OBJ_DIR}")
    return None


def validate_change_note(note):
    """验证变更记录是否合法（只允许字母、数字和下划线）"""
    if not all(c.isalnum() or c == "_" for c in note):
        raise ValueError("变更记录只允许使用字母、数字和下划线 (例如: fix_bug)")
    return note


def rename_and_copy_file(
    hex_path, version_dir, base_name, version_number, date_str, target, change_note
):
    """重命名.hex文件并复制到Version目录"""
    # 构建新文件名：XZF_v2.0.0C-r版次号_date_target_(变更记录).hex
    version_flag = TARGET_VERSION_MAP.get(target)
    log_info(
        f"👌 {target}  Board:|{version_flag[1]}|  Flag:|{version_flag[0]}|  Note:|{version_flag[2]}|"
    )

    new_name = f"{base_name}{version_flag[0]}-r{version_number}_{date_str}_{version_flag[1]}_({change_note})_{version_flag[2]}.hex"
    new_path = os.path.join(version_dir, new_name)

    # 重命名并复制文件
    shutil.copy2(hex_path, new_path)
    log_info(f"✅ 已复制: {hex_path} -> {new_path}")
    return new_path


def rename_and_copy_keil_complie_log_file(
    version_dir, base_name, version_number, date_str, target
):
    version_flag = TARGET_VERSION_MAP.get(target)
    log_info(
        f"👌 {target}  Board:|{version_flag[1]}|  Flag:|{version_flag[0]}|  Note:|{version_flag[2]}|"
    )

    new_name = f"{base_name}{version_flag[0]}-r{version_number}_{version_flag[1]}_{date_str}_keil_compile_log.txt"
    new_path = os.path.join(version_dir, new_name)

    """在OBJ目录中查找keil_compile_log文件"""
    for file in os.listdir("."):
        if file == f"keil_compile_log.txt":
            shutil.copy2(file, new_path)
            log_info(f"✅ 已复制: {file} -> {new_path}")
            return new_path


def zip_obj_folder(
    obj_dir, output_dir, base_name, version_number, date_str, target, change_note
):
    version_flag = TARGET_VERSION_MAP.get(target)
    log_info(
        f"👌 {target}  Board:|{version_flag[1]}|  Flag:|{version_flag[0]}|  Note:|{version_flag[2]}|"
    )

    """压缩OBJ目录为zip文件并存放到Version目录"""
    # 构建压缩包名：XZF_v2.0.0C+版次号_date_target_(变更记录).zip
    zip_name = f"{base_name}{version_flag[0]}-r{version_number}_{version_flag[1]}_{date_str}_({change_note})_{version_flag[2]}_output.zip"
    zip_path = os.path.join(output_dir, zip_name)

    # 创建压缩包
    with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zipf:
        for root, _, files in os.walk(obj_dir):
            for file in files:
                file_path = os.path.join(root, file)
                arcname = os.path.relpath(file_path, obj_dir)
                zipf.write(file_path, arcname)

    log_info(f"✅ 已压缩: {obj_dir} -> {zip_path}")
    return zip_path


def cleanup_old_builds(version_dir, output_dir):
    """清理Version目录中上次编译遗留的产物，仅保留release_version.txt"""
    release_log = "release_version.txt"

    # 构建每个target的精确匹配前缀 (如 QHF_v1.3.1-r 或 QHF_v1.3.1A-r)
    target_patterns = []
    for target in TARGETS:
        version_flag = TARGET_VERSION_MAP.get(target)
        if version_flag:
            flag_prefix = version_flag[0]  # "", "A", "B", "C"
            board = version_flag[1]        # "901", "906", "909"
            prefix = f"{SOFTWARE_NAME}{flag_prefix}-r"
            target_patterns.append((prefix, board))

    deleted_count = 0

    # 清理 Version 根目录
    if os.path.exists(version_dir):
        for filename in os.listdir(version_dir):
            filepath = os.path.join(version_dir, filename)
            if not os.path.isfile(filepath):
                continue
            if filename == release_log:
                continue
            for prefix, board in target_patterns:
                if filename.startswith(prefix) and f"_{board}_" in filename:
                    try:
                        os.remove(filepath)
                        deleted_count += 1
                        log_info(f"🗑️ 已删除旧产物: {filename}")
                    except Exception as e:
                        log_info(f"⚠️ 无法删除 {filename}: {e}")
                    break

    # 清理 output 子目录
    if os.path.exists(output_dir):
        for filename in os.listdir(output_dir):
            filepath = os.path.join(output_dir, filename)
            if not os.path.isfile(filepath):
                continue
            for prefix, board in target_patterns:
                if filename.startswith(prefix) and f"_{board}_" in filename:
                    try:
                        os.remove(filepath)
                        deleted_count += 1
                        log_info(f"🗑️ 已删除旧产物: output/{filename}")
                    except Exception as e:
                        log_info(f"⚠️ 无法删除 output/{filename}: {e}")
                    break

    if deleted_count == 0:
        log_info("✅ 未发现旧编译产物，无需清理")
    else:
        log_info(f"✅ 共清理 {deleted_count} 个旧编译产物")


def count_warnings():
    """从keil_compile_log.txt中解析编译警告数量"""
    log_file = "keil_compile_log.txt"
    if not os.path.exists(log_file):
        return 0
    try:
        with open(log_file, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
        # 从最后一行向上查找 Keil 汇总行: "xxx.axf" - 0 Error(s), N Warning(s).
        for line in reversed(lines):
            match = re.search(r'(\d+)\s+Warning\(s\)', line)
            if match:
                return int(match.group(1))
        return 0
    except Exception:
        return 0


def compile_target(keil_path, project_path, target):
    """调用编译工具编译指定target，返回(是否成功, 警告数)"""
    autopiler = "Keil-Autopiler.exe"

    # 使用统一日志接口记录
    log_info(f"\n🔃开始编译 target: {target}")
    log_info(f"🛠️使用工具: {autopiler} {keil_path} {project_path} {target}")

    # 调用编译工具
    try:
        result = subprocess.run(
            [autopiler, keil_path, project_path, target],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
        log_info(f"✅ 编译成功: {target}")
        warning_count = count_warnings()
        return True, warning_count
    except subprocess.CalledProcessError as e:
        log_info(f"❌ 编译失败: {target} (错误代码: {e.returncode})")
        warning_count = count_warnings()
        return False, warning_count
    except Exception as e:
        log_info(f"❌ 编译异常: {target} - {str(e)}")
        return False, 0


def main():
    # ========== 用户输入配置 (带缺省值) ==========
    log_info("\n==================== 输入提示 ====================")
    log_info("         按回车使用默认值 (括号内为默认值)")
    log_info("====================================================")

    keil_path = (
        input(
            f"👉 请输入Keil安装路径 (例如: C:/Keil5, 缺省为: {DEFAULT_KEIL_PATH}): "
        ).strip()
        or DEFAULT_KEIL_PATH
    )
    project_path = (
        input(
            f"👉 请输入工程文件路径 (例如: USER/project.uvprojx, 缺省为: {DEFAULT_PROJECT_PATH}): "
        ).strip()
        or DEFAULT_PROJECT_PATH
    )
    version_path = (
        input(
            f"👉 请输入Version文件夹路径 (例如: Version, 缺省为: {DEFAULT_VERSION_PATH}): "
        ).strip()
        or DEFAULT_VERSION_PATH
    )
    version_number = input("👉 请输入版次号 (例如: 1): ")
    # 重要：变更记录建议使用英文或数字（避免中文导致的文件名问题）
    change_note = input("👉 请输入简短变更记录 (建议使用英文或数字, 例如: fix_bug): ")

    #     # 验证变更记录
    #     try:
    #         change_note = validate_change_note(change_note)
    #     except ValueError as e:
    #         print(f"❌ 错误: {e}", flush=True)
    #         print("请重新运行脚本并输入合法的变更记录", flush=True)
    #         sys.exit(1)

    # ========== 获取当前日期 (格式: YYMMDD) ==========
    # 例如: 260327 (2026年3月27日)
    current_date = datetime.datetime.now().strftime("%y%m%d")
    log_info(f"📅当前日期: {current_date} (格式: YYMMDD)")

    # ========== 确认目录结构 ==========
    base_name = f"{SOFTWARE_NAME}"

    # 创建Version目录 (如果不存在)
    os.makedirs(version_path, exist_ok=True)
    os.makedirs(DEFAULT_OUTPUT_PATH, exist_ok=True)

    total_targets = len(TARGETS)

    # ========== 初始化日志文件 ==========
    log_file = os.path.join(version_path, "release_version.txt")
    logger = init_logger(log_file)

    # 写入日志头部信息
    log_info(
        f"=== 编译日志开始 ({datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}) ==="
    )
    log_info(f"Keil路径: {keil_path}")
    log_info(f"工程路径: {project_path}")
    log_info(f"版次号: {version_number}")
    log_info(f"变更记录: {change_note}")
    log_info(f"日期: {current_date}")
    log_info(f"Targets: {', '.join(TARGETS)}\n")

    # ========== 清理旧编译产物 ==========
    cleanup_old_builds(version_path, DEFAULT_OUTPUT_PATH)

    # 收集编译结果
    build_results = []

    # ========== 处理每个target ==========
    for idx, target in enumerate(TARGETS):
        try:
            # 0. 显示进度条
            progress = f"[{'#' * (idx+1)}{'.' * (total_targets - idx - 1)}] {idx+1}/{total_targets}"
            log_info(f"\n")
            log_info(
                f"\n**********************************************************************************************"
            )
            log_info(f"{progress} - 正在编译 target: {target}")

            # 1. 清空OBJ文件夹
            clear_obj_folder(OBJ_DIR)
            log_info(f"=========== 1 已清空OBJ目录: {OBJ_DIR}===========")

            # 2. 编译target
            success, warning_count = compile_target(
                keil_path, project_path, target)
            build_results.append((target, success, warning_count))

            if not success:
                log_info(f"⏭️ 跳过 {target} 后续步骤（编译失败）")
                continue

            log_info(
                f"=========== 2 已编译target: {target} | 警告: {warning_count} 条===========")

            # 3. 复制编译日志文件
            rename_and_copy_keil_complie_log_file(
                version_path, base_name, version_number, current_date, target
            )
            log_info(f"=========== 3 已复制编译日志文件===========")

            # 4. 查找.hex文件
            hex_file = get_hex_file(OBJ_DIR)
            if not hex_file:
                raise FileNotFoundError(f"😵未找到.hex文件 (在{OBJ_DIR}中)")
            log_info(f"=========== 4 已找到.hex文件: {hex_file}===========")

            # 5. 重命名并复制文件
            new_hex_path = rename_and_copy_file(
                hex_file,
                version_path,
                base_name,
                version_number,
                current_date,
                target,
                change_note,
            )
            log_info(f"=========== 5 已重命名并复制文件: {new_hex_path}===========")

            # 6. 压缩OBJ目录
            zip_path = zip_obj_folder(
                OBJ_DIR,
                DEFAULT_OUTPUT_PATH,
                base_name,
                version_number,
                current_date,
                target,
                change_note,
            )
            log_info(f"=========== 6 已压缩OBJ目录并复制文件: {zip_path}===========")

            # 7. 清空OBJ文件夹
            after_run_clear()
            log_info(f"=========== 7 已清除hex及log文件===========")

        except Exception as e:
            log_info(f"❌ 处理target {target} 失败: {str(e)}")
            build_results.append((target, False, 0))
            continue

    # ========== 编译结果汇总 ==========
    total = len(build_results)
    success_count = sum(1 for _, s, _ in build_results if s)
    fail_count = total - success_count
    total_warnings = sum(w for _, _, w in build_results)

    log_info(f"\n{'='*60}")
    log_info(f"📊 编译结果汇总")
    log_info(f"{'='*60}")
    log_info(f"   目标总数: {total}")
    log_info(f"   ✅ 成功: {success_count}")
    log_info(f"   ❌ 失败: {fail_count}")
    log_info(f"   ⚠️  警告: {total_warnings} 条")
    if fail_count > 0:
        log_info(f"\n❌ 失败目标列表:")
        for target, success, _ in build_results:
            if not success:
                log_info(f"   - {target}")
    if total_warnings > 0:
        log_info(f"\n⚠️  含警告的目标:")
        for target, success, w in build_results:
            if success and w > 0:
                log_info(f"   - {target}: {w} 条警告")
    log_info(f"{'='*60}")

    log_info(
        f"\n=== 编译日志结束 ({datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}) ==="
    )
    log_info("\n✅ 所有target处理完成！")
    log_info(f"💯 产物已存放在: {version_path}")
    log_info(f"📄 编译日志文件: {log_file}")
    log_info("示例文件名: XZF_v2.0.0C-r45_260402_909开机1号孔_(串口读指令添加长度限制)_(开启LTS).hex")

if __name__ == "__main__":
    main()
