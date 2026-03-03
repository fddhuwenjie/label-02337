#!/bin/bash
#
# XML to DOCX Converter 一键启动脚本
# 支持 macOS / Linux / Windows (MSYS2)
#

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/frontend-user/build"

echo "========================================"
echo "  XML to DOCX Converter"
echo "  Qt/C++ 桌面应用程序"
echo "========================================"
echo ""

# 关闭已运行的旧实例
if pgrep -x "XMLToDocxConverter" > /dev/null 2>&1; then
    echo "[INFO] 关闭旧的程序实例..."
    pkill -x "XMLToDocxConverter" 2>/dev/null || true
    sleep 1
fi

# 启动应用程序的函数
launch_app() {
    local executable="$1"
    local platform="$2"
    
    echo "[INFO] 启动程序..."
    
    case "$platform" in
        macos)
            # macOS: 使用 open 命令启动，这是最可靠的方式
            open "$executable"
            ;;
        linux)
            # Linux: 后台启动
            export QT_QPA_PLATFORM=${QT_QPA_PLATFORM:-xcb}
            "$executable" &
            disown 2>/dev/null || true
            ;;
        windows)
            # Windows: 直接启动
            "$executable" &
            ;;
    esac
    
    sleep 2
    echo "[SUCCESS] 程序已启动"
}

# 检测操作系统
if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "mingw"* ]] || [[ "$OSTYPE" == "cygwin"* ]]; then
    # Windows MSYS2 环境
    EXECUTABLE="$BUILD_DIR/XMLToDocxConverter.exe"
    PLATFORM="windows"
    
    if [ -f "$EXECUTABLE" ]; then
        echo "[INFO] 检测到已编译的程序，直接启动..."
        launch_app "$EXECUTABLE" "$PLATFORM"
        exit 0
    fi
    
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
    echo "[INFO] 首次运行，安装依赖..."
    pacman -S --needed --noconfirm \
        mingw-w64-ucrt-x86_64-gcc \
        mingw-w64-ucrt-x86_64-cmake \
        mingw-w64-ucrt-x86_64-make \
        mingw-w64-ucrt-x86_64-qt6-base \
        mingw-w64-ucrt-x86_64-qscintilla-qt6 \
        mingw-w64-ucrt-x86_64-libzip \
        mingw-w64-ucrt-x86_64-pkg-config

    mkdir -p "$BUILD_DIR" && cd "$BUILD_DIR"

    echo "[INFO] 编译中..."
    if cmake -G "MinGW Makefiles" .. && mingw32-make -j$(nproc); then
        echo ""
        echo "[SUCCESS] 编译成功"
        launch_app "$EXECUTABLE" "$PLATFORM"
    else
        echo "[ERROR] 编译失败"
        read -p "按回车键退出..."
        exit 1
    fi

elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    EXECUTABLE="$BUILD_DIR/XMLToDocxConverter"
    PLATFORM="linux"
    
    if [ -f "$EXECUTABLE" ]; then
        echo "[INFO] 检测到已编译的程序，直接启动..."
        launch_app "$EXECUTABLE" "$PLATFORM"
        exit 0
    fi
    
    echo "[INFO] 检测到 Linux 系统"
    echo "[INFO] 首次运行，需要编译..."

    if [ -f /etc/debian_version ]; then
        echo "[INFO] 安装依赖 (需要 sudo 权限)..."
        sudo apt-get update
        sudo apt-get install -y build-essential cmake pkg-config \
            qt6-base-dev libqt6xml6 libqscintilla2-qt6-dev libzip-dev
    elif [ -f /etc/redhat-release ]; then
        echo "[INFO] 检测到 RHEL/CentOS/Fedora 系统"
        echo "[INFO] 请确保已安装以下依赖:"
        echo "  sudo dnf install gcc-c++ cmake pkgconfig qt6-qtbase-devel qscintilla-qt6-devel libzip-devel"
    elif [ -f /etc/arch-release ]; then
        echo "[INFO] 检测到 Arch Linux 系统"
        echo "[INFO] 请确保已安装以下依赖:"
        echo "  sudo pacman -S base-devel cmake qt6-base qscintilla-qt6 libzip pkgconf"
    else
        echo "[WARN] 未识别的 Linux 发行版，请手动安装: cmake, pkg-config, qt6, qscintilla2, libzip"
    fi

    mkdir -p "$BUILD_DIR" && cd "$BUILD_DIR"

    echo "[INFO] 编译中..."
    if cmake .. && make -j$(nproc 2>/dev/null || echo 4); then
        echo ""
        echo "[SUCCESS] 编译成功"
        launch_app "$EXECUTABLE" "$PLATFORM"
    else
        echo "[ERROR] 编译失败"
        exit 1
    fi

elif [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    EXECUTABLE="$BUILD_DIR/XMLToDocxConverter"
    PLATFORM="macos"
    
    if [ -f "$EXECUTABLE" ]; then
        echo "[INFO] 检测到已编译的程序，直接启动..."
        launch_app "$EXECUTABLE" "$PLATFORM"
        # 保持终端打开一会儿让用户看到消息
        sleep 1
        exit 0
    fi
    
    echo "[INFO] 检测到 macOS 系统"
    echo "[INFO] 首次运行，需要编译..."

    if ! command -v brew &> /dev/null; then
        echo "[ERROR] 未检测到 Homebrew，请先安装: https://brew.sh"
        echo ""
        read -p "按回车键退出..."
        exit 1
    fi

    echo "[INFO] 安装依赖..."
    brew install cmake qt qscintilla2 libzip pkg-config

    HOMEBREW_PREFIX="$(brew --prefix)"
    export PATH="${HOMEBREW_PREFIX}/opt/qt/bin:$PATH"
    export PKG_CONFIG_PATH="${HOMEBREW_PREFIX}/opt/qt/lib/pkgconfig:${HOMEBREW_PREFIX}/lib/pkgconfig:$PKG_CONFIG_PATH"
    export CMAKE_PREFIX_PATH="${HOMEBREW_PREFIX}/opt/qt:$CMAKE_PREFIX_PATH"

    mkdir -p "$BUILD_DIR" && cd "$BUILD_DIR"

    echo "[INFO] 编译中..."
    if cmake .. && make -j$(sysctl -n hw.ncpu 2>/dev/null || echo 4); then
        echo ""
        echo "[SUCCESS] 编译成功"
        launch_app "$EXECUTABLE" "$PLATFORM"
        sleep 1
    else
        echo "[ERROR] 编译失败"
        echo ""
        read -p "按回车键退出..."
        exit 1
    fi

else
    echo "[ERROR] 不支持的操作系统: $OSTYPE"
    echo ""
    echo "支持的系统："
    echo "  - Linux (Ubuntu/Debian/Fedora/Arch)"
    echo "  - macOS (需要 Homebrew)"
    echo "  - Windows (需要 MSYS2 UCRT64)"
    exit 1
fi
