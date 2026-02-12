#!/bin/bash
#
# XML to DOCX Converter 一键启动脚本
#

echo "========================================"
echo "  XML to DOCX Converter 启动中..."
echo "========================================"
echo ""

# 获取脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# 启动 Docker 容器
echo "正在构建并启动 Docker 容器..."
docker-compose up --build -d

if [ $? -eq 0 ]; then
    echo ""
    echo "========================================"
    echo "  启动成功!"
    echo "========================================"
    echo ""
    echo "请在浏览器中访问: http://localhost:8088"
    echo ""
    echo "使用说明:"
    echo "  1. 从左侧选择示例模板或手动编写 XML"
    echo "  2. 点击 '导出 DOCX' 生成 Word 文档"
    echo ""
    echo "停止服务: docker-compose down"
    echo ""
else
    echo ""
    echo "启动失败，请检查 Docker 是否已安装并运行"
    echo ""
fi
