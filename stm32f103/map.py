import argparse
import os
import re
from typing import Dict, Tuple, Optional

def _parse_int(text: str) -> int:
    t = text.strip()
    if t.lower().startswith("0x"):
        return int(t, 16)
    if t.endswith("kB"):
        return int(float(t[:-2]) * 1024)
    if t.endswith("MB"):
        return int(float(t[:-2]) * 1024 * 1024)
    return int(t)

def parse_armcc_totals(map_path: str) -> Tuple[int, int, int, int]:
    """解析ARMCC map文件中的Grand Totals行
    格式: Code  inc.data  RO_Data  RW_Data  ZI_Data  Debug  Grand Totals
    返回: (Code, RO Data, RW Data, ZI Data)
    """
    code = 0
    ro = 0
    rw = 0
    zi = 0
    grand_totals_pattern = re.compile(r"^\s*(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+\d+\s+Grand Totals\s*$")
    with open(map_path, "r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            m = grand_totals_pattern.search(line)
            if m:
                code = int(m.group(1))
                # m.group(2) 是 inc.data，忽略
                ro = int(m.group(3))   # RO Data
                rw = int(m.group(4))   # RW Data
                zi = int(m.group(5))   # ZI Data
                break
    return code, ro, rw, zi

def parse_armcc_objects(map_path: str) -> Dict[str, Tuple[int, int]]:
    """解析ARMCC map文件中的对象文件统计信息
    返回: {文件名: (flash占用, ram占用)}
    """
    objects: Dict[str, Tuple[int, int]] = {}
    header_pattern = re.compile(r"Code \(inc\. data\)\s+RO Data\s+RW Data\s+ZI Data\s+Debug\s+Object Name")
    library_member_header = re.compile(r"Library Member Name")
    library_header = re.compile(r"Library Name")
    in_table = False
    with open(map_path, "r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            if not in_table and header_pattern.search(line):
                in_table = True
                continue
            if in_table:
                if library_member_header.search(line) or library_header.search(line):
                    break
                # 匹配 .o 或 .lib 文件
                name_m = re.search(r"([^\s]+\.(?:o|lib)[^\s]*)\s*$", line)
                if not name_m:
                    continue
                # 提取数字: Code, inc.data, RO Data, RW Data, ZI Data, Debug
                nums = re.findall(r"(\d+)", line[:name_m.start()])
                if len(nums) < 5:  # 至少需要5个数字
                    continue
                # 正确的索引: 0=Code, 1=inc.data(忽略), 2=RO Data, 3=RW Data, 4=ZI Data
                code = int(nums[0])
                ro_data = int(nums[2])
                rw_data = int(nums[3])
                zi_data = int(nums[4])
                name = name_m.group(1)
                # Flash = Code + RO Data + RW Data
                # RAM = RW Data + ZI Data
                flash = code + ro_data + rw_data
                ram = rw_data + zi_data
                objects[name] = (flash, ram)
    return objects

def parse_sct_sizes(sct_path: str) -> Tuple[Optional[int], Optional[int]]:
    rom_size = None
    ram_size = None
    rom_line = re.compile(r"LR_?IROM\w*\s+0x[0-9A-Fa-f]+\s+0x([0-9A-Fa-f]+)")
    ram_line = re.compile(r"RW_?IRAM\w*\s+0x[0-9A-Fa-f]+\s+0x([0-9A-Fa-f]+)")
    with open(sct_path, "r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            m1 = rom_line.search(line)
            if m1 and rom_size is None:
                rom_size = int(m1.group(1), 16)
            m2 = ram_line.search(line)
            if m2 and ram_size is None:
                ram_size = int(m2.group(1), 16)
    return rom_size, ram_size

def format_bytes(n: int) -> str:
    if n >= 1024 * 1024:
        return f"{n} ({n/1024/1024:.2f}MB)"
    if n >= 1024:
        return f"{n} ({n/1024:.2f}kB)"
    return str(n)

def main():
    parser = argparse.ArgumentParser(description="Parse ARMCC map file to report Flash and RAM usage")
    parser.add_argument("--map", dest="map_path", default="D:\\Program Files\\Desktop\\w5500lwip\\stm32f103_lwip\\stm32f103\\Output\\Listings\\workspace.map")
    parser.add_argument("--detail", dest="detail", action="store_true")
    parser.add_argument("--sct", dest="sct_path", default="D:\\Program Files\\Desktop\\w5500lwip\\stm32f103_lwip\\stm32f103\\Output\\workspace.sct")
    parser.add_argument("--flash-size", dest="flash_size", default=None)
    parser.add_argument("--ram-size", dest="ram_size", default=None)
    args = parser.parse_args()

    code, ro, rw, zi = parse_armcc_totals(args.map_path)
    # Flash = Code + RO Data + RW Data, RAM = RW Data + ZI Data
    total_flash = code + ro + rw
    total_ram = rw + zi

    flash_cap: Optional[int] = None
    ram_cap: Optional[int] = None
    if args.flash_size:
        flash_cap = _parse_int(args.flash_size)
    if args.ram_size:
        ram_cap = _parse_int(args.ram_size)
    if (flash_cap is None or ram_cap is None) and os.path.isfile(args.sct_path):
        s_rom, s_ram = parse_sct_sizes(args.sct_path)
        flash_cap = flash_cap if flash_cap is not None else s_rom
        ram_cap = ram_cap if ram_cap is not None else s_ram

    print("总占用:")
    print(f"Flash: {format_bytes(total_flash)}")
    print(f"RAM:   {format_bytes(total_ram)}")
    if flash_cap:
        p = total_flash / flash_cap * 100
        print(f"Flash百分比: {p:.2f}% (容量 {format_bytes(flash_cap)})")
    if ram_cap:
        p = total_ram / ram_cap * 100
        print(f"RAM百分比:   {p:.2f}% (容量 {format_bytes(ram_cap)})")

    if args.detail:
        objects = parse_armcc_objects(args.map_path)

        # 按Flash占用排序的表格
        print("\n" + "="*80)
        print("按Flash占用排序 (从大到小):")
        print("="*80)
        print(f"{'排名':<6}{'文件':<45}{'Flash':>12}{'RAM':>12}{'总计':>12}")
        print("-"*80)
        sorted_by_flash = sorted(objects.items(), key=lambda x: (-x[1][0], x[0]))
        flash_total = 0
        ram_from_flash_sort = 0
        for idx, (name, (f_use, r_use)) in enumerate(sorted_by_flash, 1):
            flash_total += f_use
            ram_from_flash_sort += r_use
            total = f_use + r_use
            print(f"{idx:<6}{name:<45}{f_use:>12}{r_use:>12}{total:>12}")
        print("-"*80)
        print(f"{'总计':<51}{flash_total:>12}{ram_from_flash_sort:>12}{flash_total + ram_from_flash_sort:>12}")

        # 按RAM占用排序的表格
        print("\n" + "="*80)
        print("按RAM占用排序 (从大到小):")
        print("="*80)
        print(f"{'排名':<6}{'文件':<45}{'Flash':>12}{'RAM':>12}{'总计':>12}")
        print("-"*80)
        sorted_by_ram = sorted(objects.items(), key=lambda x: (-x[1][1], x[0]))
        flash_from_ram_sort = 0
        ram_total = 0
        for idx, (name, (f_use, r_use)) in enumerate(sorted_by_ram, 1):
            flash_from_ram_sort += f_use
            ram_total += r_use
            total = f_use + r_use
            print(f"{idx:<6}{name:<45}{f_use:>12}{r_use:>12}{total:>12}")
        print("-"*80)
        print(f"{'总计':<51}{flash_from_ram_sort:>12}{ram_total:>12}{flash_from_ram_sort + ram_total:>12}")
        print("="*80)

if __name__ == "__main__":
    main()
