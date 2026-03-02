#ifndef DOCUMENTMODEL_H
#define DOCUMENTMODEL_H

#include <QString>
#include <QVector>
#include <QVariant>
#include <memory>

// 样式属性结构体
struct StyleAttributes {
    QString fontFamily = "Arial";
    int fontSize = 12;
    QString color = "#000000";
    QString alignment = "left";      // left, center, right, justify
    bool bold = false;
    bool italic = false;
    bool underline = false;
    int spacingBefore = 0;
    int spacingAfter = 0;
};

// 文本片段（支持行内富文本）
struct TextRun {
    QString text;
    QString fontFamily = "Arial";
    int fontSize = 12;
    QString color = "#000000";
    bool bold = false;
    bool italic = false;
    bool underline = false;
};

// 段落元素
struct ParagraphElement {
    QString text;                    // 纯文本（向后兼容）
    StyleAttributes style;           // 段落级别样式
    QVector<TextRun> runs;           // 富文本片段列表
    
    bool hasRuns() const { return !runs.isEmpty(); }
    
    QString plainText() const {
        if (runs.isEmpty()) return text;
        QString result;
        for (const auto &run : runs) {
            result += run.text;
        }
        return result;
    }
};

// 图片元素
struct ImageElement {
    QString src;
    int width = 0;      // 0 表示自动
    int height = 0;     // 0 表示自动
    QString alignment = "left";
};

// 单元格元素
struct CellElement {
    QString content;
    StyleAttributes style;
    QString alignment = "left";
    QString valign = "top";         // top, middle, bottom
    int colspan = 1;
    int rowspan = 1;
};

// 表格元素
struct TableElement {
    int rows = 0;
    int cols = 0;
    QString widthType = "auto";     // auto, fixed, percent
    int width = 0;
    QVector<int> rowHeights;
    QVector<int> colWidths;
    QVector<QVector<CellElement>> cells;
};

// 文档元素类型枚举
enum class ElementType {
    Paragraph,
    Image,
    Table
};

// 文档元素包装器
struct DocumentElement {
    ElementType type;
    ParagraphElement paragraph;
    ImageElement image;
    TableElement table;
};

// 文档模型
struct DocumentModel {
    QString title;
    QVector<DocumentElement> elements;
    
    void addParagraph(const ParagraphElement& para) {
        DocumentElement elem;
        elem.type = ElementType::Paragraph;
        elem.paragraph = para;
        elements.append(elem);
    }
    
    void addImage(const ImageElement& img) {
        DocumentElement elem;
        elem.type = ElementType::Image;
        elem.image = img;
        elements.append(elem);
    }
    
    void addTable(const TableElement& tbl) {
        DocumentElement elem;
        elem.type = ElementType::Table;
        elem.table = tbl;
        elements.append(elem);
    }
    
    void clear() {
        title.clear();
        elements.clear();
    }
};

#endif // DOCUMENTMODEL_H
