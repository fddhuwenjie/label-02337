# Requirements Document

## Introduction

本文档定义了 XML to DOCX 转换器的功能需求。该应用程序是一个基于 Qt/C++ 的桌面应用，允许用户通过 XML 配置文件定义文档结构和样式，并将其导出为 Microsoft Word DOCX 格式。应用程序使用 QScintilla 提供 XML 语法高亮编辑功能，并支持 Docker 容器化部署。

## Glossary

- **XML_Editor**: 基于 QScintilla 的 XML 编辑器组件，提供语法高亮和代码编辑功能
- **DOCX_Generator**: 负责将解析后的 XML 内容转换为 DOCX 文档的模块
- **XML_Parser**: 负责解析 XML 配置文件并提取文档结构的模块
- **Document_Element**: XML 中定义的文档元素，包括段落、图片、表格等
- **Style_Attribute**: 文档元素的样式属性，如字体、颜色、对齐方式等
- **Sample_Generator**: 生成示例 XML 文件的功能模块
- **Main_Window**: 应用程序的主窗口界面

## Requirements

### Requirement 1: XML 编辑器功能

**User Story:** 作为用户，我希望有一个支持语法高亮的 XML 编辑器，以便我能够方便地编写和修改 XML 配置文件。

#### Acceptance Criteria

1. WHEN 应用程序启动 THEN THE Main_Window SHALL 显示一个基于 QScintilla 的 XML_Editor 组件
2. WHEN 用户在 XML_Editor 中输入 XML 内容 THEN THE XML_Editor SHALL 提供 XML 语法高亮显示
3. WHEN 用户打开一个 XML 文件 THEN THE XML_Editor SHALL 加载并显示文件内容
4. WHEN 用户修改 XML 内容后点击保存 THEN THE XML_Editor SHALL 将内容保存到文件
5. WHEN 用户输入的 XML 格式不正确 THEN THE XML_Parser SHALL 显示错误提示信息

### Requirement 2: 文本样式支持

**User Story:** 作为用户，我希望能够通过 XML 配置设置文字的颜色、字体和对齐方式，以便生成格式丰富的文档。

#### Acceptance Criteria

1. WHEN XML 中定义了 font-family 属性 THEN THE DOCX_Generator SHALL 在生成的文档中应用指定的字体
2. WHEN XML 中定义了 font-size 属性 THEN THE DOCX_Generator SHALL 在生成的文档中应用指定的字号
3. WHEN XML 中定义了 color 属性 THEN THE DOCX_Generator SHALL 在生成的文档中应用指定的文字颜色
4. WHEN XML 中定义了 alignment 属性（left/center/right/justify）THEN THE DOCX_Generator SHALL 在生成的文档中应用指定的对齐方式
5. WHEN XML 中定义了 bold 或 italic 属性 THEN THE DOCX_Generator SHALL 在生成的文档中应用相应的文字样式

### Requirement 3: 图片插入功能

**User Story:** 作为用户，我希望能够在文档中插入图片并设置对齐方式，以便创建包含图文的文档。

#### Acceptance Criteria

1. WHEN XML 中定义了 image 元素 THEN THE DOCX_Generator SHALL 在生成的文档中插入指定的图片
2. WHEN image 元素包含 src 属性 THEN THE DOCX_Generator SHALL 从指定路径加载图片文件
3. WHEN image 元素包含 width 或 height 属性 THEN THE DOCX_Generator SHALL 按指定尺寸调整图片大小
4. WHEN image 元素包含 alignment 属性 THEN THE DOCX_Generator SHALL 按指定方式对齐图片
5. IF 指定的图片文件不存在 THEN THE DOCX_Generator SHALL 显示错误提示并跳过该图片

### Requirement 4: 表格功能

**User Story:** 作为用户，我希望能够在文档中插入表格并设置各种表格属性，以便创建结构化的数据展示。

#### Acceptance Criteria

1. WHEN XML 中定义了 table 元素 THEN THE DOCX_Generator SHALL 在生成的文档中创建表格
2. WHEN table 元素包含 rows 和 cols 属性 THEN THE DOCX_Generator SHALL 创建指定行列数的表格
3. WHEN table 元素包含 width 属性设为 auto THEN THE DOCX_Generator SHALL 使表格宽度自适应内容
4. WHEN row 元素包含 height 属性 THEN THE DOCX_Generator SHALL 设置指定的行高
5. WHEN col 元素包含 width 属性 THEN THE DOCX_Generator SHALL 设置指定的列宽
6. WHEN cell 元素包含 alignment 属性 THEN THE DOCX_Generator SHALL 设置单元格内容的对齐方式
7. WHEN cell 元素包含 valign 属性 THEN THE DOCX_Generator SHALL 设置单元格内容的垂直对齐方式

### Requirement 5: XML 配置格式

**User Story:** 作为用户，我希望所有文档设置都使用 XML 格式存储，以便我能够方便地编辑和版本控制配置文件。

#### Acceptance Criteria

1. THE XML_Parser SHALL 支持解析符合预定义 schema 的 XML 配置文件
2. WHEN XML 文件包含 document 根元素 THEN THE XML_Parser SHALL 正确识别文档结构
3. WHEN XML 文件包含嵌套元素 THEN THE XML_Parser SHALL 正确解析层级关系
4. WHEN XML 文件包含样式属性 THEN THE XML_Parser SHALL 提取并传递给 DOCX_Generator
5. FOR ALL 有效的 XML 配置文件，解析后再序列化 SHALL 产生语义等价的 XML 内容（round-trip property）

### Requirement 6: 示例生成功能

**User Story:** 作为用户，我希望能够生成示例 XML 文件，以便我能够快速了解 XML 配置格式和功能。

#### Acceptance Criteria

1. WHEN 用户点击"生成示例"按钮 THEN THE Sample_Generator SHALL 在 XML_Editor 中显示示例 XML 内容
2. THE Sample_Generator SHALL 提供至少 5 种不同类型的示例模板
3. WHEN 生成基础示例 THEN THE Sample_Generator SHALL 包含段落和基本文字样式
4. WHEN 生成表格示例 THEN THE Sample_Generator SHALL 包含完整的表格配置
5. WHEN 生成图片示例 THEN THE Sample_Generator SHALL 包含图片插入配置

### Requirement 7: DOCX 导出功能

**User Story:** 作为用户，我希望能够将 XML 配置导出为 DOCX 文档，以便我能够在 Microsoft Word 中查看和编辑。

#### Acceptance Criteria

1. WHEN 用户点击"导出 DOCX"按钮 THEN THE DOCX_Generator SHALL 生成有效的 DOCX 文件
2. WHEN 导出成功 THEN THE Main_Window SHALL 显示成功提示并提供文件保存路径
3. IF 导出过程中发生错误 THEN THE DOCX_Generator SHALL 显示详细的错误信息
4. THE DOCX_Generator SHALL 生成符合 OOXML 标准的 DOCX 文件
5. WHEN 生成的 DOCX 文件在 Microsoft Word 中打开 THEN 文档内容 SHALL 与 XML 配置一致

### Requirement 8: Docker 容器化部署

**User Story:** 作为运维人员，我希望应用程序能够通过 Docker 容器化部署，以便在不同环境中快速部署和运行。

#### Acceptance Criteria

1. THE Dockerfile SHALL 支持构建 ARM64 和 AMD64 架构的镜像
2. WHEN 执行 docker-compose up --build -d THEN 应用程序 SHALL 正确启动
3. THE 应用程序 SHALL 通过端口 8088 提供 Web 访问
4. THE 项目根目录 SHALL 包含 docker-compose.yml、.gitignore 和 README.md 文件
5. THE README.md SHALL 包含 How to Run、Services、测试账号和题目内容说明

### Requirement 9: 项目结构规范

**User Story:** 作为开发者，我希望项目遵循标准的 C++ 项目结构，以便于维护和扩展。

#### Acceptance Criteria

1. THE 项目 SHALL 使用 CMake 作为构建系统
2. THE 源代码 SHALL 存放在 src/ 目录下
3. THE 头文件 SHALL 存放在 include/ 目录下
4. THE 项目根目录 SHALL 包含 CMakeLists.txt 构建配置文件
5. THE 项目 SHALL 使用 Qt 5 或 Qt 6 Widgets 框架
6. THE 项目 SHALL 使用 QScintilla 作为编辑器组件
