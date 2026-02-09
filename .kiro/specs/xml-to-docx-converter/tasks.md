# Implementation Plan: XML to DOCX Converter

## Overview

本实现计划将 XML to DOCX 转换器分解为可执行的编码任务。项目使用 Qt/C++ 开发，采用 QScintilla 作为 XML 编辑器，使用 libzip 和手动构建 OOXML 生成 DOCX 文件。

## Tasks

- [x] 1. 项目初始化和基础结构
  - [x] 1.1 创建 CMake 项目结构
    - 创建 frontend-user/src/ 和 frontend-user/include/ 目录
    - 创建 CMakeLists.txt 配置 Qt6、QScintilla、libzip 依赖
    - 配置 C++17 标准和编译选项
    - _Requirements: 9.1, 9.2, 9.3, 9.4, 9.5, 9.6_

  - [x] 1.2 创建数据模型类
    - 在 include/ 下创建 DocumentModel.h
    - 定义 StyleAttributes、ParagraphElement、ImageElement、TableElement、CellElement 结构体
    - 定义 DocumentModel 类包含所有文档元素
    - _Requirements: 5.1_

- [x] 2. XML 解析器实现
  - [x] 2.1 实现 XMLParser 类
    - 创建 include/XMLParser.h 和 src/XMLParser.cpp
    - 实现 parse() 方法解析 XML 字符串为 DocumentModel
    - 实现 validate() 方法验证 XML 格式
    - 实现 serialize() 方法将 DocumentModel 序列化为 XML
    - _Requirements: 5.1, 5.2, 5.3, 5.4_

  - [ ]* 2.2 编写 XMLParser 属性测试
    - **Property 1: XML Round-Trip Consistency**
    - **Validates: Requirements 5.5**

  - [ ]* 2.3 编写 XMLParser 单元测试
    - 测试有效 XML 解析
    - 测试无效 XML 错误处理
    - **Property 5: Invalid XML Error Detection**
    - **Validates: Requirements 1.5**

- [ ] 3. Checkpoint - 确保 XML 解析器测试通过
  - 确保所有测试通过，如有问题请询问用户

- [x] 4. DOCX 生成器实现
  - [x] 4.1 实现 OOXML 基础结构生成
    - 创建 include/DOCXGenerator.h 和 src/DOCXGenerator.cpp
    - 实现 createDocxStructure() 创建临时目录结构
    - 实现 writeContentTypes() 生成 [Content_Types].xml
    - 实现 writeRelationships() 生成 _rels/.rels
    - _Requirements: 7.4_

  - [x] 4.2 实现文档内容生成
    - 实现 writeDocument() 生成 word/document.xml
    - 实现 writeParagraph() 生成段落 OOXML
    - 实现 styleToOOXML() 转换样式属性
    - _Requirements: 2.1, 2.2, 2.3, 2.4, 2.5_

  - [x] 4.3 实现表格生成
    - 实现 writeTable() 生成表格 OOXML
    - 支持行高、列宽、单元格对齐设置
    - _Requirements: 4.1, 4.2, 4.3, 4.4, 4.5, 4.6, 4.7_

  - [x] 4.4 实现图片嵌入
    - 实现 writeImage() 生成图片 OOXML
    - 实现图片关系管理和嵌入
    - 支持图片尺寸和对齐设置
    - _Requirements: 3.1, 3.2, 3.3, 3.4, 3.5_

  - [x] 4.5 实现 DOCX 打包
    - 实现 packageDocx() 使用 libzip 打包为 DOCX
    - 实现 generate() 整合所有步骤
    - _Requirements: 7.1_

  - [ ]* 4.6 编写 DOCXGenerator 属性测试
    - **Property 2: Style Attributes Preservation**
    - **Property 3: Table Structure Integrity**
    - **Property 4: Image Embedding Correctness**
    - **Property 6: DOCX Format Validity**
    - **Validates: Requirements 2.1-2.5, 3.1-3.4, 4.1-4.7, 7.1, 7.4**

- [ ] 5. Checkpoint - 确保 DOCX 生成器测试通过
  - 确保所有测试通过，如有问题请询问用户

- [x] 6. 示例生成器实现
  - [x] 6.1 实现 SampleGenerator 类
    - 创建 include/SampleGenerator.h 和 src/SampleGenerator.cpp
    - 实现 generateBasicText() 生成基础文本示例
    - 实现 generateRichText() 生成富文本示例
    - 实现 generateTableSample() 生成表格示例
    - 实现 generateImageSample() 生成图片示例
    - 实现 generateCompleteSample() 生成完整示例
    - _Requirements: 6.1, 6.2, 6.3, 6.4, 6.5_

- [x] 7. 主窗口 UI 实现
  - [x] 7.1 实现 MainWindow 基础框架
    - 创建 include/MainWindow.h 和 src/MainWindow.cpp
    - 实现 setupUI() 创建主窗口布局
    - 实现 setupMenuBar() 创建菜单栏
    - 实现 setupToolBar() 创建工具栏
    - _Requirements: 1.1_

  - [x] 7.2 集成 QScintilla 编辑器
    - 实现 setupEditor() 配置 QScintilla
    - 配置 XML 语法高亮 (QsciLexerXML)
    - 配置行号显示和代码折叠
    - _Requirements: 1.1, 1.2_

  - [x] 7.3 实现文件操作功能
    - 实现 onNewFile() 新建文件
    - 实现 onOpenFile() 打开文件
    - 实现 onSaveFile() 保存文件
    - _Requirements: 1.3, 1.4_

  - [x] 7.4 实现导出和示例功能
    - 实现 onExportDocx() 导出 DOCX
    - 实现 onGenerateSample() 生成示例
    - 集成错误提示和成功提示
    - _Requirements: 6.1, 7.1, 7.2, 7.3_

  - [x] 7.5 创建 main.cpp 入口文件
    - 创建 src/main.cpp
    - 初始化 QApplication 和 MainWindow
    - _Requirements: 1.1_

- [ ] 8. Checkpoint - 确保应用程序可以正常运行
  - 确保所有测试通过，如有问题请询问用户

- [x] 9. Docker 容器化
  - [x] 9.1 创建 Dockerfile
    - 基于支持 ARM64/AMD64 的基础镜像
    - 安装 Qt6、QScintilla、libzip 依赖
    - 配置 CMake 构建
    - 配置 VNC/noVNC 用于 Web 访问
    - _Requirements: 8.1, 8.3_

  - [x] 9.2 更新 docker-compose.yml
    - 配置 frontend-user 服务
    - 映射端口 8088
    - 配置卷挂载
    - _Requirements: 8.2, 8.3_

  - [x] 9.3 更新项目文档
    - 更新 README.md 包含 How to Run、Services、测试账号、题目内容
    - 更新 .gitignore 排除构建产物
    - _Requirements: 8.4, 8.5_

- [ ] 10. Final Checkpoint - 确保 Docker 部署正常
  - 执行 docker-compose up --build -d 验证部署
  - 确保所有测试通过，如有问题请询问用户

## Notes

- 任务标记 `*` 为可选测试任务，可跳过以加快 MVP 开发
- 每个任务都引用了具体的需求编号以便追溯
- Checkpoint 任务用于阶段性验证
- 属性测试验证通用正确性属性
- 单元测试验证具体示例和边界情况
