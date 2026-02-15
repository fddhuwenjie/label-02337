#!/bin/bash
#
# XML to DOCX Converter 一键启动脚本
# Qt/C++ 桌面应用程序
#

echo "========================================"
echo "  XML to DOCX Converter"
echo "  Qt/C++ 桌面应用程序"
echo "========================================"
echo ""

# 检测操作系统
if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "mingw"* ]]; then
    # Windows MSYS2 环境
    if ! command -v pacman &> /dev/null; then
        echo "[错误] 请使用 MSYS2 UCRT64 终端运行此脚本"
        echo ""
        echo "Windows 安装步骤："
        echo "  1. 下载 MSYS2: https://www.msys2.org/"
        echo "  2. 安装后，从开始菜单打开 'MSYS2 UCRT64'"
        echo "  3. 进入项目目录: cd /c/path/to/project"
        echo "  4. 运行: ./start.sh"
        echo ""
        read -p "按回车键退出..."
        exit 1
    fi
    
    echo "[INFO] 检测到 MSYS2 环境"
    echo "[INFO] 安装依赖..."
    pacman -S --needed --noconfirm \
        mingw-w64-ucrt-x86_64-gcc \
        mingw-w64-ucrt-x86_64-cmake \
        mingw-w64-ucrt-x86_64-make \
        mingw-w64-ucrt-x86_64-qt6-base \
        mingw-w64-ucrt-x86_64-qscintilla-qt6 \
        mingw-w64-ucrt-x86_64-libzip \
        mingw-w64-ucrt-x86_64-pkg-config
    
    cd frontend-user
    rm -rf build && mkdir -p build && cd build
    
    echo "[INFO] 编译中..."
    cmake -G "MinGW Makefiles" .. && mingw32-make -j$(nproc)
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "[SUCCESS] 编译成功，启动程序..."
        ./XMLToDocxConverter.exe
    else
        echo "[ERROR] 编译失败"
        read -p "按回车键退出..."
        exit 1
    fi

elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    echo "[INFO] 检测到 Linux 系统"
    
    if [ -f /etc/debian_version ]; then
        echo "[INFO] 安装依赖 (需要 sudo 权限)..."
        sudo apt-get update
        sudo apt-get install -y build-essential cmake pkg-config \
            qt6-base-dev libqt6xml6 libqscintilla2-qt6-dev libzip-dev
    fi
    
    cd frontend-user
    rm -rf build && mkdir -p build && cd build
    
    echo "[INFO] 编译中..."
    cmake .. && make -j$(nproc)
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "[SUCCESS] 编译成功，启动程序..."
        ./XMLToDocxConverter
    else
        echo "[ERROR] 编译失败"
        exit 1
    fi

elif [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    echo "[INFO] 检测到 macOS 系统"
    
    if command -v brew &> /dev/null; then
        echo "[INFO] 安装依赖..."
        brew install cmake qt@6 qscintilla2 libzip
        export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"
    fi
    
    cd frontend-user
    rm -rf build && mkdir -p build && cd build
    
    echo "[INFO] 编译中..."
    cmake .. && make -j$(sysctl -n hw.ncpu)
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "[SUCCESS] 编译成功，启动程序..."
        ./XMLToDocxConverter
    else
        echo "[ERROR] 编译失败"
        exit 1
    fi

else
    echo "[ERROR] 不支持的操作系统: $OSTYPE"
    echo ""
    echo "支持的系统："
    echo "  - Linux (Ubuntu/Debian)"
    echo "  - macOS (需要 Homebrew)"
    echo "  - Windows (需要 MSYS2 UCRT64)"
    exit 1
fi
