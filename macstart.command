#!/bin/bash
#
# XML to DOCX Converter 启动脚本 (macOS)
#

cd "$(dirname "$0")"

EXECUTABLE="./frontend-user/build/XMLToDocxConverter"

echo "========================================"
echo "  XML to DOCX Converter"
echo "========================================"
echo ""

# 关闭已运行的旧实例
if pgrep -x "XMLToDocxConverter" > /dev/null 2>&1; then
    echo "[INFO] 关闭旧的程序实例..."
    pkill -x "XMLToDocxConverter" 2>/dev/null
    sleep 1
fi

# 检查是否已编译
if [ ! -f "$EXECUTABLE" ]; then
    echo "[INFO] 首次运行，需要编译..."
    echo "[INFO] 请在终端中运行: ./start.sh"
    echo ""
    read -p "按回车键退出..."
    exit 1
fi

echo "[INFO] 启动程序..."

# 使用 open 命令启动（macOS 原生方式）
open "$EXECUTABLE"

echo "[SUCCESS] 程序已启动"
sleep 2
