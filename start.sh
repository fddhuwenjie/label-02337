#!/bin/bash
#
# XML to DOCX Converter 一键启动脚本
# 支持 Linux / macOS / Windows (MSYS2/Git Bash)
#

set -e

echo "========================================"
echo "  XML to DOCX Converter"
echo "========================================"
echo ""

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/frontend-user"

# 检测操作系统
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ -n "$MSYSTEM" ]]; then
        echo "windows"
    else
        echo "unknown"
    fi
}

OS=$(detect_os)
echo "检测到操作系统: $OS"
echo ""

# Windows (MSYS2) 环境安装依赖
install_windows_deps() {
    echo "正在检查 MSYS2 环境..."
    
    if ! command -v pacman &> /dev/null; then
        echo ""
        echo "错误: 未检测到 MSYS2 环境"
        echo ""
        echo "请安装 MSYS2："
        echo "1. 下载: https://www.msys2.org/"
        echo "2. 安装后，从开始菜单打开 'MSYS2 UCRT64'"
        echo "3. 在 MSYS2 终端中运行此脚本"
        echo ""
        read -p "按 Enter 键退出..."
        exit 1
    fi
    
    echo "正在安装依赖 (需要管理员权限)..."
    
    # 更新包数据库
    pacman -Sy --noconfirm
    
    # 安装编译工具和依赖
    pacman -S --noconfirm --needed \
        mingw-w64-ucrt-x86_64-gcc \
        mingw-w64-ucrt-x86_64-cmake \
        mingw-w64-ucrt-x86_64-make \
        mingw-w64-ucrt-x86_64-qt6-base \
        mingw-w64-ucrt-x86_64-qscintilla-qt6 \
        mingw-w64-ucrt-x86_64-libzip \
        mingw-w64-ucrt-x86_64-pkg-config \
        make
    
    echo "依赖安装完成"
}

# Linux 环境安装依赖
install_linux_deps() {
    echo "正在检查依赖..."
    
    if command -v apt-get &> /dev/null; then
        echo "检测到 Debian/Ubuntu 系统"
        sudo apt-get update
        sudo apt-get install -y build-essential cmake pkg-config \
            qt6-base-dev libqt6xml6 libqscintilla2-qt6-dev libzip-dev
    elif command -v dnf &> /dev/null; then
        echo "检测到 Fedora/RHEL 系统"
        sudo dnf install -y gcc-c++ cmake pkgconfig \
            qt6-qtbase-devel qscintilla-qt6-devel libzip-devel
    elif command -v pacman &> /dev/null; then
        echo "检测到 Arch Linux 系统"
        sudo pacman -Sy --noconfirm base-devel cmake qt6-base qscintilla-qt6 libzip
    else
        echo "警告: 无法自动安装依赖，请手动安装"
    fi
}

# macOS 环境安装依赖
install_macos_deps() {
    echo "正在检查依赖..."
    
    if ! command -v brew &> /dev/null; then
        echo "正在安装 Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    
    brew install cmake qt@6 qscintilla2 libzip pkg-config
}

# 编译项目
build_project() {
    echo ""
    echo "正在编译项目..."
    
    mkdir -p build
    cd build
    
    if [[ "$OS" == "windows" ]]; then
        # Windows MSYS2 使用 mingw32-make
        cmake -G "MinGW Makefiles" ..
        mingw32-make -j$(nproc)
    else
        cmake ..
        make -j$(nproc)
    fi
    
    echo "编译完成"
}

# 运行程序
run_app() {
    echo ""
    echo "正在启动程序..."
    
    cd "$SCRIPT_DIR/frontend-user/build"
    
    if [[ "$OS" == "windows" ]]; then
        ./XMLToDocxConverter.exe
    else
        ./XMLToDocxConverter
    fi
}

# 主流程
main() {
    # 安装依赖
    case $OS in
        linux)
            install_linux_deps
            ;;
        macos)
            install_macos_deps
            ;;
        windows)
            install_windows_deps
            ;;
        *)
            echo "不支持的操作系统: $OSTYPE"
            exit 1
            ;;
    esac
    
    # 编译
    build_project
    
    # 运行
    run_app
}

main

