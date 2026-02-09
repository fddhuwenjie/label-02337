#include "SampleGenerator.h"

SampleGenerator::SampleGenerator() {}

QStringList SampleGenerator::sampleNames() const {
    return QStringList{
        "基础文本示例",
        "富文本示例",
        "表格示例",
        "图片示例",
        "完整示例"
    };
}

QString SampleGenerator::generate(SampleType type) {
    switch (type) {
        case BasicText:
            return generateBasicText();
        case RichText:
            return generateRichText();
        case TableSample:
            return generateTableSample();
        case ImageSample:
            return generateImageSample();
        case CompleteSample:
            return generateCompleteSample();
        default:
            return generateBasicText();
    }
}

QString SampleGenerator::generateBasicText() {
    return R"(<?xml version="1.0" encoding="UTF-8"?>
<document title="基础文本示例">
    <!-- 标题 -->
    <paragraph align="center" spacingAfter="300">
        <text font="Arial" size="24" color="#1a5276" bold="true">
            基础文本格式演示
        </text>
    </paragraph>

    <!-- 普通段落 -->
    <paragraph align="left" spacingAfter="200">
        <text font="Arial" size="12" color="#000000">
            这是一段普通的左对齐文本，展示了基本的段落格式。
        </text>
    </paragraph>

    <!-- 居中段落 -->
    <paragraph align="center" spacingAfter="200">
        <text font="Arial" size="12" color="#666666" italic="true">
            这是居中对齐的斜体文本
        </text>
    </paragraph>

    <!-- 右对齐段落 -->
    <paragraph align="right" spacingAfter="200">
        <text font="Arial" size="10" color="#999999">
            右对齐文本 - 2024年
        </text>
    </paragraph>
</document>
)";
}

QString SampleGenerator::generateRichText() {
    return R"(<?xml version="1.0" encoding="UTF-8"?>
<document title="富文本示例">
    <!-- 彩色标题 -->
    <paragraph align="center" spacingAfter="400">
        <text font="Arial" size="28" color="#e74c3c" bold="true">
            富文本格式演示
        </text>
    </paragraph>

    <!-- 彩虹色文字 -->
    <paragraph align="center" spacingAfter="200">
        <text font="Arial" size="16" color="#e74c3c" bold="true">红</text>
    </paragraph>
    <paragraph align="center" spacingAfter="200">
        <text font="Arial" size="16" color="#f39c12" bold="true">橙</text>
    </paragraph>
    <paragraph align="center" spacingAfter="200">
        <text font="Arial" size="16" color="#f1c40f" bold="true">黄</text>
    </paragraph>
    <paragraph align="center" spacingAfter="200">
        <text font="Arial" size="16" color="#2ecc71" bold="true">绿</text>
    </paragraph>
    <paragraph align="center" spacingAfter="200">
        <text font="Arial" size="16" color="#3498db" bold="true">蓝</text>
    </paragraph>
    <paragraph align="center" spacingAfter="200">
        <text font="Arial" size="16" color="#9b59b6" bold="true">紫</text>
    </paragraph>

    <!-- 混合样式 -->
    <paragraph align="left" spacingAfter="200">
        <text font="Times New Roman" size="14" color="#2c3e50" bold="true" italic="true">
            粗体斜体文本 - Times New Roman 字体
        </text>
    </paragraph>

    <paragraph align="left" spacingAfter="200">
        <text font="Courier New" size="12" color="#27ae60" underline="true">
            下划线等宽字体文本 - Courier New
        </text>
    </paragraph>
</document>
)";
}

QString SampleGenerator::generateTableSample() {
    return R"(<?xml version="1.0" encoding="UTF-8"?>
<document title="表格示例">
    <!-- 标题 -->
    <paragraph align="center" spacingAfter="300">
        <text font="Arial" size="20" color="#2c3e50" bold="true">
            产品价格表
        </text>
    </paragraph>

    <!-- 产品表格 -->
    <table rows="5" cols="4" width="auto">
        <row height="35">
            <cell align="center" bold="true" color="#ffffff" style="background:#3498db">产品名称</cell>
            <cell align="center" bold="true" color="#ffffff">规格</cell>
            <cell align="center" bold="true" color="#ffffff">单价</cell>
            <cell align="center" bold="true" color="#ffffff">库存</cell>
        </row>
        <row height="30">
            <cell align="left">笔记本电脑</cell>
            <cell align="center">15.6寸</cell>
            <cell align="right">¥5,999</cell>
            <cell align="center">128</cell>
        </row>
        <row height="30">
            <cell align="left">无线鼠标</cell>
            <cell align="center">标准版</cell>
            <cell align="right">¥99</cell>
            <cell align="center">500</cell>
        </row>
        <row height="30">
            <cell align="left">机械键盘</cell>
            <cell align="center">87键</cell>
            <cell align="right">¥299</cell>
            <cell align="center">256</cell>
        </row>
        <row height="30">
            <cell align="left">显示器</cell>
            <cell align="center">27寸 4K</cell>
            <cell align="right">¥2,499</cell>
            <cell align="center">64</cell>
        </row>
    </table>

    <!-- 备注 -->
    <paragraph align="left" spacingBefore="200">
        <text font="Arial" size="10" color="#7f8c8d" italic="true">
            * 价格仅供参考，以实际销售价格为准
        </text>
    </paragraph>
</document>
)";
}

QString SampleGenerator::generateImageSample() {
    return R"(<?xml version="1.0" encoding="UTF-8"?>
<document title="图片示例">
    <!-- 标题 -->
    <paragraph align="center" spacingAfter="300">
        <text font="Arial" size="20" color="#2c3e50" bold="true">
            图片插入演示
        </text>
    </paragraph>

    <!-- 说明文字 -->
    <paragraph align="left" spacingAfter="200">
        <text font="Arial" size="12" color="#333333">
            以下展示了图片插入功能，支持设置图片的宽度、高度和对齐方式。
        </text>
    </paragraph>

    <!-- 居中图片 -->
    <image src="./images/sample.png" width="300" height="200" align="center"/>

    <!-- 图片说明 -->
    <paragraph align="center" spacingBefore="100" spacingAfter="300">
        <text font="Arial" size="10" color="#7f8c8d" italic="true">
            图1: 示例图片（居中对齐，300x200像素）
        </text>
    </paragraph>

    <!-- 左对齐图片 -->
    <image src="./images/logo.png" width="150" align="left"/>

    <paragraph align="left" spacingBefore="100">
        <text font="Arial" size="10" color="#7f8c8d" italic="true">
            图2: Logo图片（左对齐，宽度150像素）
        </text>
    </paragraph>
</document>
)";
}

QString SampleGenerator::generateCompleteSample() {
    return R"(<?xml version="1.0" encoding="UTF-8"?>
<document title="项目报告">
    <!-- 报告标题 -->
    <paragraph align="center" spacingAfter="400">
        <text font="Arial" size="28" color="#1a5276" bold="true">
            2024年度项目总结报告
        </text>
    </paragraph>

    <!-- 副标题 -->
    <paragraph align="center" spacingAfter="300">
        <text font="Arial" size="14" color="#7f8c8d">
            技术研发部 | 2024年12月
        </text>
    </paragraph>

    <!-- 章节标题 -->
    <paragraph align="left" spacingBefore="300" spacingAfter="200">
        <text font="Arial" size="18" color="#2c3e50" bold="true">
            一、项目概述
        </text>
    </paragraph>

    <paragraph align="left" spacingAfter="200">
        <text font="Arial" size="12" color="#333333">
            本年度共完成5个重点项目的研发工作，涵盖前端开发、后端服务、数据分析等多个技术领域。团队成员通力合作，按时完成了所有里程碑目标。
        </text>
    </paragraph>

    <!-- 项目统计表格 -->
    <paragraph align="left" spacingBefore="200" spacingAfter="100">
        <text font="Arial" size="14" color="#2c3e50" bold="true">
            项目完成情况统计
        </text>
    </paragraph>

    <table rows="4" cols="3" width="auto">
        <row height="35">
            <cell align="center" bold="true">项目名称</cell>
            <cell align="center" bold="true">完成状态</cell>
            <cell align="center" bold="true">完成时间</cell>
        </row>
        <row height="30">
            <cell align="left">用户管理系统</cell>
            <cell align="center" color="#27ae60">已完成</cell>
            <cell align="center">2024-03</cell>
        </row>
        <row height="30">
            <cell align="left">数据分析平台</cell>
            <cell align="center" color="#27ae60">已完成</cell>
            <cell align="center">2024-06</cell>
        </row>
        <row height="30">
            <cell align="left">移动端应用</cell>
            <cell align="center" color="#27ae60">已完成</cell>
            <cell align="center">2024-09</cell>
        </row>
    </table>

    <!-- 总结 -->
    <paragraph align="left" spacingBefore="300" spacingAfter="200">
        <text font="Arial" size="18" color="#2c3e50" bold="true">
            二、总结与展望
        </text>
    </paragraph>

    <paragraph align="left" spacingAfter="200">
        <text font="Arial" size="12" color="#333333">
            2024年是充满挑战与收获的一年。展望2025年，我们将继续深耕技术创新，为公司发展贡献更大力量。
        </text>
    </paragraph>

    <!-- 页脚 -->
    <paragraph align="right" spacingBefore="400">
        <text font="Arial" size="10" color="#999999">
            报告生成时间: 2024-12-31
        </text>
    </paragraph>
</document>
)";
}
