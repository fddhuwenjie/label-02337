#!/bin/bash
#
# XML to DOCX Converter 一键启动脚本
# 
# 使用方法：
#   chmod +x start.sh
#   ./start.sh
#

echo "========================================"
echo "  XML to DOCX Converter"
echo "========================================"
echo ""

# 检测是否为 Windows 环境
if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ -n "$WINDIR" ]]; then
    echo "检测到 Windows 系统"
    echo ""
    echo "Windows 用户请使用 Docker 方式运行："
    echo ""
    echo "步骤 1: 确保 Docker Desktop 已安装并运行"
    echo ""
    echo "步骤 2: 在此目录打开 PowerShell 或 CMD，执行："
    echo "        docker-compose up --build -d"
    echo ""
    echo "步骤 3: 等待构建完成后，浏览器访问:"
    echo "        http://localhost:8088"
    echo ""
    echo "========================================"
    echo ""
    # 使用多种方式确保窗口不会立即关闭
    echo "按 Enter 键退出..."
    read
    exit 0
fi

# Linux/macOS 环境
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/frontend-user"

chmod +x run.sh
./run.sh || {
    echo ""
    echo "运行失败，按 Enter 键退出..."
    read
    exit 1
}
