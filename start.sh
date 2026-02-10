#!/bin/bash
#
# XML to DOCX Converter 一键启动脚本
# 
# 使用方法：
#   chmod +x start.sh
#   ./start.sh
#

set -e

echo "========================================"
echo "  XML to DOCX Converter"
echo "========================================"
echo ""

# 获取脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/frontend-user"

# 执行运行脚本
chmod +x run.sh
./run.sh
