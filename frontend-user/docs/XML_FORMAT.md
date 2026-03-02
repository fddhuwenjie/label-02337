# XML 配置格式文档

本文档详细说明了 XML to DOCX Converter 使用的 XML 配置格式。

## 文档结构

```xml
<?xml version="1.0" encoding="UTF-8"?>
<document title="文档标题">
    <!-- 文档内容元素 -->
</document>
```

### 根元素 `<document>`

| 属性 | 类型 | 必需 | 说明 |
|------|------|------|------|
| title | string | 否 | 文档标题 |

## 段落元素 `<paragraph>`

用于定义文本段落。

```xml
<paragraph align="center" spacingBefore="100" spacingAfter="200">
    <text font="Arial" size="14" color="#333333" bold="true">
        段落文本内容
    </text>
</paragraph>
```

### 段落属性

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| align | string | left | 对齐方式: left, center, right, justify |
| spacingBefore | int | 0 | 段前间距 (twips, 1/20 磅) |
| spacingAfter | int | 0 | 段后间距 (twips) |

### 文本元素 `<text>`

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| font | string | Arial | 字体名称 |
| size | int | 12 | 字号 (磅) |
| color | string | #000000 | 文字颜色 (十六进制) |
| bold | bool | false | 是否加粗 |
| italic | bool | false | 是否斜体 |
| underline | bool | false | 是否下划线 |

## 图片元素 `<image>`

用于插入图片。

```xml
<image src="./images/photo.png" width="300" height="200" align="center"/>
```

### 图片属性

| 属性 | 类型 | 必需 | 说明 |
|------|------|------|------|
| src | string | 是 | 图片路径 (相对或绝对路径) |
| width | int | 否 | 图片宽度 (像素)，0 表示自动 |
| height | int | 否 | 图片高度 (像素)，0 表示自动 |
| align | string | 否 | 对齐方式: left, center, right |

### 支持的图片格式

- PNG (.png)
- JPEG (.jpg, .jpeg)
- GIF (.gif)
- BMP (.bmp)

## 表格元素 `<table>`

用于创建表格。

```xml
<table rows="3" cols="4" width="auto">
    <row height="35">
        <cell align="center" valign="middle" bold="true">表头1</cell>
        <cell align="center" valign="middle" bold="true">表头2</cell>
        <cell align="center" valign="middle" bold="true">表头3</cell>
        <cell align="center" valign="middle" bold="true">表头4</cell>
    </row>
    <row height="30">
        <cell align="left">数据1</cell>
        <cell align="center">数据2</cell>
        <cell align="right">数据3</cell>
        <cell>数据4</cell>
    </row>
</table>
```

### 表格属性

| 属性 | 类型 | 必需 | 说明 |
|------|------|------|------|
| rows | int | 是 | 行数 |
| cols | int | 是 | 列数 |
| width | string/int | 否 | 表格宽度: "auto" 或具体数值 (twips) |

### 行元素 `<row>`

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| height | int | 0 | 行高 (磅)，0 表示自动 |

### 单元格元素 `<cell>`

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| align | string | left | 水平对齐: left, center, right |
| valign | string | top | 垂直对齐: top, middle, bottom |
| colspan | int | 1 | 跨列数 |
| rowspan | int | 1 | 跨行数 |
| bold | bool | false | 是否加粗 |
| color | string | #000000 | 文字颜色 |

## 完整示例

```xml
<?xml version="1.0" encoding="UTF-8"?>
<document title="示例报告">
    <!-- 标题 -->
    <paragraph align="center" spacingAfter="400">
        <text font="Arial" size="28" color="#1a5276" bold="true">
            年度工作报告
        </text>
    </paragraph>

    <!-- 副标题 -->
    <paragraph align="center" spacingAfter="300">
        <text font="Arial" size="14" color="#7f8c8d">
            2024年度 | 技术部
        </text>
    </paragraph>

    <!-- 正文段落 -->
    <paragraph align="left" spacingAfter="200">
        <text font="Arial" size="12" color="#333333">
            本年度共完成多个重点项目，取得了显著成果。
        </text>
    </paragraph>

    <!-- 图片 -->
    <image src="./chart.png" width="400" align="center"/>

    <!-- 数据表格 -->
    <table rows="4" cols="3" width="auto">
        <row height="35">
            <cell align="center" bold="true">项目</cell>
            <cell align="center" bold="true">状态</cell>
            <cell align="center" bold="true">完成时间</cell>
        </row>
        <row height="30">
            <cell align="left">项目A</cell>
            <cell align="center" color="#27ae60">已完成</cell>
            <cell align="center">2024-03</cell>
        </row>
        <row height="30">
            <cell align="left">项目B</cell>
            <cell align="center" color="#27ae60">已完成</cell>
            <cell align="center">2024-06</cell>
        </row>
        <row height="30">
            <cell align="left">项目C</cell>
            <cell align="center" color="#f39c12">进行中</cell>
            <cell align="center">-</cell>
        </row>
    </table>

    <!-- 页脚 -->
    <paragraph align="right" spacingBefore="400">
        <text font="Arial" size="10" color="#999999">
            生成时间: 2024-12-31
        </text>
    </paragraph>
</document>
```

## 颜色值

支持以下颜色格式：

1. **十六进制**: `#RRGGBB` 或 `#RGB`
   - 例: `#FF0000`, `#F00`

2. **颜色名称**:
   - black, white, red, green, blue
   - yellow, orange, purple, pink
   - gray/grey, silver, navy, teal
   - maroon, olive, lime, aqua, cyan, fuchsia

## 字体

推荐使用以下跨平台字体：

- Arial (默认)
- Times New Roman
- Courier New
- Verdana
- Georgia
- SimSun (宋体)
- SimHei (黑体)
- Microsoft YaHei (微软雅黑)

## 注意事项

1. XML 文件必须使用 UTF-8 编码
2. 所有属性值需要用引号包围
3. 图片路径支持相对路径和绝对路径
4. 表格的 rows 和 cols 属性必须与实际行列数匹配
5. 颜色值不区分大小写
