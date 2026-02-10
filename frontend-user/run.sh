#!/bin/bash
#
# XML to DOCX Converter 一键运行脚本
# 
# 功能：自动检测环境、安装依赖、编译并运行程序
#
# 使用方法：
#   chmod +x run.sh
#   ./run.sh
#

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 打印带颜色的消息
info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
    exit 1
}

# 检测操作系统
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if [ -f /etc/debian_version ]; then
            OS="debian"
        elif [ -f /etc/redhat-release ]; then
            OS="redhat"
        elif [ -f /etc/arch-release ]; then
            OS="arch"
        else
            OS="linux"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
    else
        OS="unknown"
    fi
    info "检测到操作系统: $OS"
}

# 检查并安装依赖
install_dependencies() {
    info "检查依赖..."
    
    case $OS in
        debian)
            # Debian/Ubuntu
            PACKAGES="build-essential cmake pkg-config qt6-base-dev libqt6xml6 libqscintilla2-qt6-dev libzip-dev"
            
            # 检查是否需要安装
            NEED_INSTALL=false
            for pkg in $PACKAGES; do
                if ! dpkg -l | grep -q "^ii  $pkg"; then
                    NEED_INSTALL=true
                    break
                fi
            done
            
            if [ "$NEED_INSTALL" = true ]; then
                warn "需要安装依赖包，可能需要 sudo 权限"
                sudo apt-get update
                sudo apt-get install -y $PACKAGES
            else
                info "所有依赖已安装"
            fi
            ;;
            
        redhat)
            # RHEL/CentOS/Fedora
            PACKAGES="gcc-c++ cmake pkgconfig qt6-qtbase-devel qscintilla-qt6-devel libzip-devel"
            warn "请确保已安装: $PACKAGES"
            warn "可使用: sudo dnf install $PACKAGES"
            ;;
            
        arch)
            # Arch Linux
            PACKAGES="base-devel cmake qt6-base qscintilla-qt6 libzip"
            warn "请确保已安装: $PACKAGES"
            warn "可使用: sudo pacman -S $PACKAGES"
            ;;
            
        macos)
            # macOS
            if ! command -v brew &> /dev/null; then
                error "请先安装 Homebrew: https://brew.sh"
            fi
            
            PACKAGES="cmake qt@6 qscintilla2 libzip"
            for pkg in $PACKAGES; do
                if ! brew list $pkg &> /dev/null; then
                    info "安装 $pkg..."
                    brew install $pkg
                fi
            done
            
            # 设置 Qt 路径
            export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"
            export PKG_CONFIG_PATH="/opt/homebrew/opt/qt@6/lib/pkgconfig:$PKG_CONFIG_PATH"
            ;;
            
        *)
            warn "未知操作系统，请手动安装以下依赖:"
            warn "  - CMake >= 3.16"
            warn "  - Qt6 (Core, Widgets, Xml)"
            warn "  - QScintilla2 for Qt6"
            warn "  - libzip"
            ;;
    esac
}

# 编译项目
build_project() {
    info "开始编译项目..."
    
    # 获取脚本所在目录
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    cd "$SCRIPT_DIR"
    
    # 创建构建目录
    if [ -d "build" ]; then
        info "清理旧的构建目录..."
        rm -rf build
    fi
    
    mkdir -p build
    cd build
    
    # 运行 CMake
    info "运行 CMake..."
    cmake .. || error "CMake 配置失败"
    
    # 编译
    info "编译中..."
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4) || error "编译失败"
    
    info "编译成功!"
}

# 运行程序
run_program() {
    info "启动程序..."
    
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    EXECUTABLE="$SCRIPT_DIR/build/XMLToDocxConverter"
    
    if [ ! -f "$EXECUTABLE" ]; then
        error "可执行文件不存在: $EXECUTABLE"
    fi
    
    # 设置环境变量
    export QT_QPA_PLATFORM=${QT_QPA_PLATFORM:-xcb}
    
    # 运行
    "$EXECUTABLE"
}

# 主函数
main() {
    echo "========================================"
    echo "  XML to DOCX Converter 一键运行脚本"
    echo "========================================"
    echo ""
    
    detect_os
    install_dependencies
    build_project
    run_program
}

# 执行主函数
main "$@"
