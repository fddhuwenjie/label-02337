# XML to DOCX Converter

基于 Qt/C++ 的 XML 配置文件导出 DOCX 文档工具，支持丰富的格式设置。

## How to Run

### 一键运行脚本（推荐）

**Linux / Windows:**
```bash
chmod +x start.sh
./start.sh
```

**macOS（推荐）:**
```bash
# 首次运行需要先编译
./start.sh

# 之后可以双击 macstart.command 启动程序
# 或在终端运行:
./macstart.command
```

> macOS 用户推荐使用 `macstart.command`，双击即可启动程序。

脚本会自动检测系统、安装依赖、编译并运行程序。

#### Windows 用户

Windows 需要使用 MSYS2 环境运行：

1. 下载安装 MSYS2: https://www.msys2.org/
2. 从开始菜单打开 **MSYS2 UCRT64**
3. 进入项目目录并运行脚本：
   ```bash
   cd /c/path/to/project
   ./start.sh
   ```

### 手动编译运行

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install build-essential cmake pkg-config qt6-base-dev libqt6xml6 libqscintilla2-qt6-dev libzip-dev
cd frontend-user && mkdir build && cd build
cmake .. && make -j$(nproc)
./XMLToDocxConverter
```

**Windows (MSYS2 UCRT64):**
```bash
pacman -S mingw-w64-ucrt-x86_64-{gcc,cmake,make,qt6-base,qscintilla-qt6,libzip,pkg-config}
cd frontend-user && mkdir build && cd build
cmake -G "MinGW Makefiles" .. && mingw32-make -j$(nproc)
./XMLToDocxConverter.exe
```

## Services

| 服务名 | 端口 | 描述 |
|--------|------|------|
| frontend-user | 8088 | Qt 桌面应用 (通过 noVNC 访问) |

## 测试账号

本项目为桌面应用，无需登录账号。

## 题目内容

请帮我使用QT/C++编写一个将静态网页导出成DOCX文档的程序，需要支持文字颜色，字体，对齐属性设置。能够插入图片，图片能够设置对齐。能够插入表格，支持表格的行列数设置，表格宽度自适应设置，行高列宽设置，单元格内容的对齐设置。这些设置项需要使用xml格式存储。我需要一个QT Widget的程序，除完成功能之外，请帮我编写一个生成示例，生成一个示例的xml文件，然后支持导出为DOCX文档。界面上需要有一个编辑窗口，能够编辑该XML文档，编辑器使用QScintilla实现。

## 功能特性

- **文字格式**: 支持颜色、字体、对齐方式设置
- **图片插入**: 支持图片插入及对齐设置
- **表格支持**: 行列数、宽度自适应、行高列宽、单元格对齐、单元格合并
- **XML配置**: 所有设置项使用 XML 格式存储
- **HTML导入**: 支持导入 HTML 文件并转换为 XML 格式
- **代码编辑器**: 使用 QScintilla 实现语法高亮的 XML 编辑器
- **DOCX导出**: 支持导出为标准 DOCX 文档
- **日志记录**: 完整的日志记录功能，便于调试和问题追踪

## 依赖要求

### 必需依赖

| 依赖 | 最低版本 | 说明 |
|------|----------|------|
| CMake | 3.16+ | 构建系统 |
| Qt6 | 6.2+ | GUI 框架 (Core, Widgets, Xml 模块) |
| QScintilla2 | 2.13+ | 代码编辑器组件 (Qt6 版本) |
| libzip | 1.7+ | ZIP 文件处理库 |
| GCC/Clang | C++17 支持 | 编译器 |

### 各平台安装命令

**Ubuntu/Debian:**
```bash
sudo apt-get install build-essential cmake pkg-config qt6-base-dev libqt6xml6 libqscintilla2-qt6-dev libzip-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install gcc-c++ cmake pkgconfig qt6-qtbase-devel qscintilla-qt6-devel libzip-devel
```

**Arch Linux:**
```bash
sudo pacman -S base-devel cmake qt6-base qscintilla-qt6 libzip
```

**macOS (Homebrew):**
```bash
brew install cmake qt@6 qscintilla2 libzip
```

## 技术栈

- **语言**: C++17
- **GUI框架**: Qt 6 Widgets
- **编辑器组件**: QScintilla
- **DOCX生成**: libzip + OOXML
- **构建系统**: CMake
- **容器化**: Docker (支持 ARM64/AMD64)

## 项目结构

```
.
├── frontend-user/          # Qt 桌面应用
│   ├── src/               # C++ 源代码
│   │   ├── main.cpp
│   │   ├── MainWindow.cpp
│   │   ├── XMLParser.cpp
│   │   ├── HTMLParser.cpp
│   │   ├── DOCXGenerator.cpp
│   │   ├── SampleGenerator.cpp
│   │   └── Logger.cpp
│   ├── include/           # 头文件
│   │   ├── MainWindow.h
│   │   ├── XMLParser.h
│   │   ├── HTMLParser.h
│   │   ├── DOCXGenerator.h
│   │   ├── SampleGenerator.h
│   │   ├── DocumentModel.h
│   │   └── Logger.h
│   ├── samples/           # 示例 XML 文件
│   ├── docs/              # 文档
│   │   └── XML_FORMAT.md  # XML 格式说明
│   ├── CMakeLists.txt     # CMake 构建配置
│   └── Dockerfile         # Docker 构建文件
├── docker-compose.yml     # Docker Compose 配置
├── start.sh              # 跨平台一键启动脚本
├── macstart.command      # macOS 双击启动脚本
├── .gitignore            # Git 忽略文件
└── README.md             # 项目说明
```

## XML 配置格式示例

```xml
<?xml version="1.0" encoding="UTF-8"?>
<document title="示例文档">
    <paragraph align="center" spacingAfter="300">
        <text font="Arial" size="24" color="#FF0000" bold="true">
            标题文本
        </text>
    </paragraph>
    
    <image src="image.png" align="center" width="400" height="300"/>
    
    <table rows="3" cols="3" width="auto">
        <row height="30">
            <cell align="center" valign="middle">单元格内容</cell>
            <cell align="left">左对齐</cell>
            <cell align="right">右对齐</cell>
        </row>
    </table>
</document>
```

## 本地开发

```bash
# 安装依赖 (Ubuntu/Debian)
sudo apt-get install build-essential cmake qt6-base-dev libqscintilla2-qt6-dev libzip-dev

# 构建
cd frontend-user
mkdir build && cd build
cmake ..
make

# 运行
./XMLToDocxConverter
```


## 详细文档

- [XML 格式说明](frontend-user/docs/XML_FORMAT.md) - 详细的 XML 配置格式文档

## 使用说明

### 基本操作

1. **新建文件**: 点击工具栏"新建"按钮或按 `Ctrl+N`
2. **打开文件**: 点击"打开"按钮或按 `Ctrl+O` 打开 XML 文件
3. **导入 HTML**: 点击"导入 HTML"或按 `Ctrl+Shift+O` 导入 HTML 文件
4. **保存文件**: 点击"保存"按钮或按 `Ctrl+S`
5. **导出 DOCX**: 点击"导出 DOCX"按钮或按 `Ctrl+E`

### 示例模板

应用内置 5 种示例模板：
- 基础文本示例 - 展示基本文字格式
- 富文本示例 - 混合格式和彩色文字
- 表格示例 - 产品价格表
- 图片示例 - 图片插入演示
- 完整示例 - 项目报告模板

### 快捷键

| 快捷键 | 功能 |
|--------|------|
| Ctrl+N | 新建文件 |
| Ctrl+O | 打开文件 |
| Ctrl+Shift+O | 导入 HTML |
| Ctrl+S | 保存文件 |
| Ctrl+Shift+S | 另存为 |
| Ctrl+E | 导出 DOCX |
| Ctrl+T | 验证 XML |
| Ctrl+Z | 撤销 |
| Ctrl+Y | 重做 |

## 错误处理

应用提供详细的错误信息：

- **XML 解析错误**: 显示错误行号和具体错误描述
- **图片加载错误**: 提示图片文件不存在或格式不支持
- **DOCX 生成错误**: 显示生成过程中的具体错误

日志文件位置：
- Windows: `%APPDATA%/DocX Studio/xml2docx.log`
- Linux: `~/.local/share/DocX Studio/xml2docx.log`
- macOS: `~/Library/Application Support/DocX Studio/xml2docx.log`
