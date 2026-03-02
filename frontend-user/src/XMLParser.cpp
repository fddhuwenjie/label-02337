#include "XMLParser.h"
#include <QDebug>

XMLParser::XMLParser() {}

XMLParser::~XMLParser() {}

std::unique_ptr<DocumentModel> XMLParser::parse(const QString &xmlContent) {
    auto model = std::make_unique<DocumentModel>();
    
    QXmlStreamReader reader(xmlContent);
    
    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        
        if (token == QXmlStreamReader::StartElement) {
            if (reader.name().toString() == "document") {
                QXmlStreamAttributes attrs = reader.attributes();
                if (attrs.hasAttribute("title")) {
                    model->title = attrs.value("title").toString();
                }
                parseDocument(reader, *model);
            }
        }
    }
    
    if (reader.hasError()) {
        m_lastError = QString("XML 解析错误 (行 %1): %2")
            .arg(reader.lineNumber())
            .arg(reader.errorString());
        return nullptr;
    }
    
    return model;
}

bool XMLParser::validate(const QString &xmlContent, QString &errorMessage) {
    QXmlStreamReader reader(xmlContent);
    
    while (!reader.atEnd()) {
        reader.readNext();
    }
    
    if (reader.hasError()) {
        errorMessage = QString("行 %1: %2")
            .arg(reader.lineNumber())
            .arg(reader.errorString());
        return false;
    }
    
    errorMessage = "XML 格式正确";
    return true;
}

void XMLParser::parseDocument(QXmlStreamReader &reader, DocumentModel &model) {
    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        
        if (token == QXmlStreamReader::EndElement && 
            reader.name().toString() == "document") {
            break;
        }
        
        if (token == QXmlStreamReader::StartElement) {
            QString elementName = reader.name().toString();
            
            if (elementName == "paragraph") {
                ParagraphElement para;
                parseParagraph(reader, para);
                model.addParagraph(para);
            }
            else if (elementName == "image") {
                ImageElement image;
                parseImage(reader, image);
                model.addImage(image);
            }
            else if (elementName == "table") {
                TableElement table;
                parseTable(reader, table);
                model.addTable(table);
            }
        }
    }
}

void XMLParser::parseParagraph(QXmlStreamReader &reader, ParagraphElement &para) {
    QXmlStreamAttributes attrs = reader.attributes();
    
    // 解析段落级别属性
    if (attrs.hasAttribute("align")) {
        para.style.alignment = attrs.value("align").toString();
    }
    if (attrs.hasAttribute("spacingBefore")) {
        para.style.spacingBefore = attrs.value("spacingBefore").toInt();
    }
    if (attrs.hasAttribute("spacingAfter")) {
        para.style.spacingAfter = attrs.value("spacingAfter").toInt();
    }
    
    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        
        if (token == QXmlStreamReader::EndElement && 
            reader.name().toString() == "paragraph") {
            break;
        }
        
        if (token == QXmlStreamReader::StartElement) {
            if (reader.name().toString() == "text") {
                parseText(reader, para);
            }
        }
        else if (token == QXmlStreamReader::Characters) {
            QString text = reader.text().toString().trimmed();
            if (!text.isEmpty()) {
                para.text += text;
            }
        }
    }
}


void XMLParser::parseText(QXmlStreamReader &reader, ParagraphElement &para) {
    QXmlStreamAttributes attrs = reader.attributes();
    StyleAttributes textStyle = parseStyleAttributes(attrs);
    
    // 如果段落样式未设置，使用 text 的样式作为段落样式
    if (para.style.fontFamily == "Arial" && textStyle.fontFamily != "Arial") {
        para.style.fontFamily = textStyle.fontFamily;
    }
    if (para.style.fontSize == 12 && textStyle.fontSize != 12) {
        para.style.fontSize = textStyle.fontSize;
    }
    if (para.style.color == "#000000" && textStyle.color != "#000000") {
        para.style.color = textStyle.color;
    }
    para.style.bold = para.style.bold || textStyle.bold;
    para.style.italic = para.style.italic || textStyle.italic;
    para.style.underline = para.style.underline || textStyle.underline;
    
    // 创建 TextRun
    TextRun run;
    run.fontFamily = textStyle.fontFamily;
    run.fontSize = textStyle.fontSize;
    run.color = textStyle.color;
    run.bold = textStyle.bold;
    run.italic = textStyle.italic;
    run.underline = textStyle.underline;
    
    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        
        if (token == QXmlStreamReader::EndElement && 
            reader.name().toString() == "text") {
            break;
        }
        
        if (token == QXmlStreamReader::Characters) {
            run.text += reader.text().toString().trimmed();
        }
    }
    
    if (!run.text.isEmpty()) {
        para.runs.append(run);
        para.text = para.plainText();
    }
}

StyleAttributes XMLParser::parseStyleAttributes(const QXmlStreamAttributes &attrs) {
    StyleAttributes style;
    
    if (attrs.hasAttribute("font")) {
        style.fontFamily = attrs.value("font").toString();
    }
    if (attrs.hasAttribute("size")) {
        style.fontSize = attrs.value("size").toInt();
    }
    if (attrs.hasAttribute("color")) {
        style.color = colorToHex(attrs.value("color").toString());
    }
    if (attrs.hasAttribute("align")) {
        style.alignment = attrs.value("align").toString();
    }
    if (attrs.hasAttribute("bold")) {
        style.bold = attrs.value("bold").toString().toLower() == "true";
    }
    if (attrs.hasAttribute("italic")) {
        style.italic = attrs.value("italic").toString().toLower() == "true";
    }
    if (attrs.hasAttribute("underline")) {
        style.underline = attrs.value("underline").toString().toLower() == "true";
    }
    
    return style;
}

QString XMLParser::colorToHex(const QString &color) {
    if (color.startsWith("#")) {
        return color;
    }
    // 支持常见颜色名称
    static QMap<QString, QString> colorMap = {
        {"red", "#FF0000"},
        {"green", "#00FF00"},
        {"blue", "#0000FF"},
        {"black", "#000000"},
        {"white", "#FFFFFF"},
        {"yellow", "#FFFF00"},
        {"orange", "#FFA500"},
        {"purple", "#800080"},
        {"gray", "#808080"},
        {"grey", "#808080"}
    };
    return colorMap.value(color.toLower(), color);
}

void XMLParser::parseImage(QXmlStreamReader &reader, ImageElement &image) {
    QXmlStreamAttributes attrs = reader.attributes();
    
    if (attrs.hasAttribute("src")) {
        image.src = attrs.value("src").toString();
    }
    if (attrs.hasAttribute("width")) {
        image.width = attrs.value("width").toInt();
    }
    if (attrs.hasAttribute("height")) {
        image.height = attrs.value("height").toInt();
    }
    if (attrs.hasAttribute("align")) {
        image.alignment = attrs.value("align").toString();
    }
    
    // 跳过到结束标签
    reader.skipCurrentElement();
}

void XMLParser::parseTable(QXmlStreamReader &reader, TableElement &table) {
    QXmlStreamAttributes attrs = reader.attributes();
    
    if (attrs.hasAttribute("rows")) {
        table.rows = attrs.value("rows").toInt();
    }
    if (attrs.hasAttribute("cols")) {
        table.cols = attrs.value("cols").toInt();
    }
    if (attrs.hasAttribute("width")) {
        QString widthStr = attrs.value("width").toString();
        if (widthStr == "auto") {
            table.widthType = "auto";
        } else {
            table.widthType = "fixed";
            table.width = widthStr.toInt();
        }
    }
    
    // 初始化单元格数组和行高列宽
    table.cells.resize(table.rows);
    for (int i = 0; i < table.rows; ++i) {
        table.cells[i].resize(table.cols);
    }
    table.rowHeights.resize(table.rows, 0);
    table.colWidths.resize(table.cols, 0);
    
    // 解析列宽属性 (colWidths="120,200,100,100")
    if (attrs.hasAttribute("colWidths")) {
        QString colWidthsStr = attrs.value("colWidths").toString();
        QStringList widthList = colWidthsStr.split(",");
        for (int i = 0; i < widthList.size() && i < table.cols; ++i) {
            table.colWidths[i] = widthList[i].trimmed().toInt();
        }
    }
    
    int currentRow = 0;
    
    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        
        if (token == QXmlStreamReader::EndElement && 
            reader.name().toString() == "table") {
            break;
        }
        
        if (token == QXmlStreamReader::StartElement) {
            if (reader.name().toString() == "row") {
                if (currentRow < table.rows) {
                    parseRow(reader, table, currentRow);
                    currentRow++;
                }
            }
        }
    }
}

void XMLParser::parseRow(QXmlStreamReader &reader, TableElement &table, int rowIndex) {
    QXmlStreamAttributes attrs = reader.attributes();
    
    if (attrs.hasAttribute("height")) {
        table.rowHeights[rowIndex] = attrs.value("height").toInt();
    }
    
    int currentCol = 0;
    
    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        
        if (token == QXmlStreamReader::EndElement && 
            reader.name().toString() == "row") {
            break;
        }
        
        if (token == QXmlStreamReader::StartElement) {
            if (reader.name().toString() == "cell") {
                if (currentCol < table.cols) {
                    parseCell(reader, table.cells[rowIndex][currentCol]);
                    currentCol++;
                }
            }
        }
    }
}

void XMLParser::parseCell(QXmlStreamReader &reader, CellElement &cell) {
    QXmlStreamAttributes attrs = reader.attributes();
    
    if (attrs.hasAttribute("align")) {
        cell.alignment = attrs.value("align").toString();
    }
    if (attrs.hasAttribute("valign")) {
        cell.valign = attrs.value("valign").toString();
    }
    if (attrs.hasAttribute("colspan")) {
        cell.colspan = attrs.value("colspan").toInt();
    }
    if (attrs.hasAttribute("rowspan")) {
        cell.rowspan = attrs.value("rowspan").toInt();
    }
    
    cell.style = parseStyleAttributes(attrs);
    
    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        
        if (token == QXmlStreamReader::EndElement && 
            reader.name().toString() == "cell") {
            break;
        }
        
        if (token == QXmlStreamReader::Characters) {
            cell.content += reader.text().toString().trimmed();
        }
    }
}

QString XMLParser::serialize(const DocumentModel &model) {
    QString output;
    QXmlStreamWriter writer(&output);
    
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(4);
    
    writer.writeStartDocument();
    writer.writeStartElement("document");
    
    if (!model.title.isEmpty()) {
        writer.writeAttribute("title", model.title);
    }
    
    for (const auto &elem : model.elements) {
        switch (elem.type) {
            case ElementType::Paragraph:
                serializeParagraph(writer, elem.paragraph);
                break;
            case ElementType::Image:
                serializeImage(writer, elem.image);
                break;
            case ElementType::Table:
                serializeTable(writer, elem.table);
                break;
        }
    }
    
    writer.writeEndElement(); // document
    writer.writeEndDocument();
    
    return output;
}

void XMLParser::serializeParagraph(QXmlStreamWriter &writer, const ParagraphElement &para) {
    writer.writeStartElement("paragraph");
    
    if (para.style.alignment != "left") {
        writer.writeAttribute("align", para.style.alignment);
    }
    if (para.style.spacingAfter > 0) {
        writer.writeAttribute("spacingAfter", QString::number(para.style.spacingAfter));
    }
    
    // 如果有 TextRun 列表，逐个序列化
    if (para.hasRuns()) {
        for (const auto &run : para.runs) {
            writer.writeStartElement("text");
            if (run.fontFamily != "Arial") {
                writer.writeAttribute("font", run.fontFamily);
            }
            if (run.fontSize != 12) {
                writer.writeAttribute("size", QString::number(run.fontSize));
            }
            if (run.color != "#000000") {
                writer.writeAttribute("color", run.color);
            }
            if (run.bold) {
                writer.writeAttribute("bold", "true");
            }
            if (run.italic) {
                writer.writeAttribute("italic", "true");
            }
            if (run.underline) {
                writer.writeAttribute("underline", "true");
            }
            writer.writeCharacters(run.text);
            writer.writeEndElement(); // text
        }
    } else {
        // 向后兼容：只有单个 text
        writer.writeStartElement("text");
        serializeStyleAttributes(writer, para.style);
        writer.writeCharacters(para.text);
        writer.writeEndElement(); // text
    }
    
    writer.writeEndElement(); // paragraph
}

void XMLParser::serializeImage(QXmlStreamWriter &writer, const ImageElement &image) {
    writer.writeEmptyElement("image");
    writer.writeAttribute("src", image.src);
    if (image.width > 0) {
        writer.writeAttribute("width", QString::number(image.width));
    }
    if (image.height > 0) {
        writer.writeAttribute("height", QString::number(image.height));
    }
    if (image.alignment != "left") {
        writer.writeAttribute("align", image.alignment);
    }
}

void XMLParser::serializeTable(QXmlStreamWriter &writer, const TableElement &table) {
    writer.writeStartElement("table");
    writer.writeAttribute("rows", QString::number(table.rows));
    writer.writeAttribute("cols", QString::number(table.cols));
    
    if (table.widthType == "auto") {
        writer.writeAttribute("width", "auto");
    } else if (table.width > 0) {
        writer.writeAttribute("width", QString::number(table.width));
    }
    
    // 序列化列宽属性
    bool hasColWidths = false;
    QStringList colWidthsList;
    for (int i = 0; i < table.colWidths.size(); ++i) {
        if (table.colWidths[i] > 0) {
            colWidthsList.append(QString::number(table.colWidths[i]));
            hasColWidths = true;
        }
    }
    if (hasColWidths) {
        writer.writeAttribute("colWidths", colWidthsList.join(","));
    }
    
    for (int i = 0; i < table.rows; ++i) {
        writer.writeStartElement("row");
        if (i < table.rowHeights.size() && table.rowHeights[i] > 0) {
            writer.writeAttribute("height", QString::number(table.rowHeights[i]));
        }
        
        for (int j = 0; j < table.cols; ++j) {
            const CellElement &cell = table.cells[i][j];
            writer.writeStartElement("cell");
            
            if (cell.alignment != "left") {
                writer.writeAttribute("align", cell.alignment);
            }
            if (cell.valign != "top") {
                writer.writeAttribute("valign", cell.valign);
            }
            
            writer.writeCharacters(cell.content);
            writer.writeEndElement(); // cell
        }
        
        writer.writeEndElement(); // row
    }
    
    writer.writeEndElement(); // table
}

void XMLParser::serializeStyleAttributes(QXmlStreamWriter &writer, const StyleAttributes &style) {
    if (style.fontFamily != "Arial") {
        writer.writeAttribute("font", style.fontFamily);
    }
    if (style.fontSize != 12) {
        writer.writeAttribute("size", QString::number(style.fontSize));
    }
    if (style.color != "#000000") {
        writer.writeAttribute("color", style.color);
    }
    if (style.bold) {
        writer.writeAttribute("bold", "true");
    }
    if (style.italic) {
        writer.writeAttribute("italic", "true");
    }
    if (style.underline) {
        writer.writeAttribute("underline", "true");
    }
}
