#include "HTMLParser.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

HTMLParser::HTMLParser() {}

HTMLParser::~HTMLParser() {}

std::unique_ptr<DocumentModel> HTMLParser::parseFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = QString("无法打开文件: %1").arg(file.errorString());
        return nullptr;
    }
    
    QFileInfo info(filePath);
    m_basePath = info.absolutePath();
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    QString content = in.readAll();
    file.close();
    
    return parse(content);
}

std::unique_ptr<DocumentModel> HTMLParser::parse(const QString &htmlContent) {
    auto model = std::make_unique<DocumentModel>();
    
    // 提取 <style> 标签中的 CSS
    QRegularExpression styleRe(R"(<style[^>]*>(.*?)</style>)", 
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator styleIt = styleRe.globalMatch(htmlContent);
    while (styleIt.hasNext()) {
        QRegularExpressionMatch match = styleIt.next();
        // 简单解析 CSS 类
        QString css = match.captured(1);
        QRegularExpression classRe(R"(\.([a-zA-Z_-]+)\s*\{([^}]*)\})");
        QRegularExpressionMatchIterator classIt = classRe.globalMatch(css);
        while (classIt.hasNext()) {
            QRegularExpressionMatch classMatch = classIt.next();
            QString className = classMatch.captured(1);
            QString styles = classMatch.captured(2);
            m_cssClasses[className] = parseInlineStyle(styles);
        }
    }
    
    // 提取 <title>
    QRegularExpression titleRe(R"(<title[^>]*>(.*?)</title>)", 
        QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch titleMatch = titleRe.match(htmlContent);
    if (titleMatch.hasMatch()) {
        model->title = stripHtmlTags(titleMatch.captured(1)).trimmed();
    }
    
    // 提取 <body> 内容
    QString body = htmlContent;
    QRegularExpression bodyRe(R"(<body[^>]*>(.*?)</body>)", 
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch bodyMatch = bodyRe.match(htmlContent);
    if (bodyMatch.hasMatch()) {
        body = bodyMatch.captured(1);
    }
    
    // 解析各种 HTML 元素
    // 标题 h1-h6
    for (int i = 1; i <= 6; ++i) {
        QRegularExpression hRe(QString(R"(<h%1([^>]*)>(.*?)</h%1>)").arg(i), 
            QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatchIterator it = hRe.globalMatch(body);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            parseHeadingTag(i, match.captured(1), match.captured(2), *model);
        }
    }
    
    // 段落 <p>
    QRegularExpression pRe(R"(<p([^>]*)>(.*?)</p>)", 
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator pIt = pRe.globalMatch(body);
    while (pIt.hasNext()) {
        QRegularExpressionMatch match = pIt.next();
        parseParagraphTag(match.captured(1), match.captured(2), *model);
    }
    
    // 图片 <img>
    QRegularExpression imgRe(R"(<img([^>]*)/??>)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator imgIt = imgRe.globalMatch(body);
    while (imgIt.hasNext()) {
        QRegularExpressionMatch match = imgIt.next();
        parseImageTag(match.captured(1), *model);
    }
    
    // 表格 <table>
    QRegularExpression tableRe(R"(<table([^>]*)>(.*?)</table>)", 
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator tableIt = tableRe.globalMatch(body);
    while (tableIt.hasNext()) {
        QRegularExpressionMatch match = tableIt.next();
        parseTableTag(match.captured(2), *model);
    }
    
    return model;
}

StyleAttributes HTMLParser::parseInlineStyle(const QString &styleStr) {
    StyleAttributes style;
    
    QStringList props = styleStr.split(';', Qt::SkipEmptyParts);
    for (const QString &prop : props) {
        QStringList parts = prop.split(':');
        if (parts.size() != 2) continue;
        
        QString name = parts[0].trimmed().toLower();
        QString value = parts[1].trimmed();
        
        if (name == "font-family") {
            style.fontFamily = value.remove('"').remove('\'');
        } else if (name == "font-size") {
            // 解析 px, pt, em 等单位
            value = value.toLower();
            if (value.endsWith("px")) {
                style.fontSize = value.chopped(2).toInt() * 3 / 4;  // px to pt
            } else if (value.endsWith("pt")) {
                style.fontSize = value.chopped(2).toInt();
            } else if (value.endsWith("em")) {
                style.fontSize = value.chopped(2).toDouble() * 12;
            } else {
                style.fontSize = value.toInt();
            }
        } else if (name == "color") {
            style.color = colorNameToHex(value);
        } else if (name == "text-align") {
            style.alignment = value.toLower();
        } else if (name == "font-weight") {
            style.bold = (value == "bold" || value.toInt() >= 700);
        } else if (name == "font-style") {
            style.italic = (value == "italic" || value == "oblique");
        } else if (name == "text-decoration") {
            style.underline = value.contains("underline");
        }
    }
    
    return style;
}

StyleAttributes HTMLParser::parseCssClass(const QString &className) {
    return m_cssClasses.value(className, StyleAttributes());
}

void HTMLParser::parseHeadingTag(int level, const QString &attrs, const QString &content, DocumentModel &model) {
    ParagraphElement para;
    
    // 根据标题级别设置默认样式
    static const int headingSizes[] = {28, 24, 20, 18, 16, 14};
    para.style.fontSize = headingSizes[level - 1];
    para.style.bold = true;
    para.style.alignment = "left";
    para.style.spacingAfter = 200;
    
    // 解析 class 属性
    QString classAttr = extractAttribute(attrs, "class");
    if (!classAttr.isEmpty()) {
        StyleAttributes classStyle = parseCssClass(classAttr);
        // 合并样式
        if (classStyle.fontSize != 12) para.style.fontSize = classStyle.fontSize;
        if (classStyle.color != "#000000") para.style.color = classStyle.color;
        if (classStyle.alignment != "left") para.style.alignment = classStyle.alignment;
    }
    
    // 解析 style 属性
    QString styleAttr = extractAttribute(attrs, "style");
    if (!styleAttr.isEmpty()) {
        StyleAttributes inlineStyle = parseInlineStyle(styleAttr);
        if (inlineStyle.fontSize != 12) para.style.fontSize = inlineStyle.fontSize;
        if (inlineStyle.color != "#000000") para.style.color = inlineStyle.color;
        if (inlineStyle.alignment != "left") para.style.alignment = inlineStyle.alignment;
    }
    
    para.text = stripHtmlTags(content).trimmed();
    model.addParagraph(para);
}

void HTMLParser::parseParagraphTag(const QString &attrs, const QString &content, DocumentModel &model) {
    ParagraphElement para;
    para.style.spacingAfter = 150;
    
    // 解析 align 属性
    QString alignAttr = extractAttribute(attrs, "align");
    if (!alignAttr.isEmpty()) {
        para.style.alignment = alignAttr.toLower();
    }
    
    // 解析 class 属性
    QString classAttr = extractAttribute(attrs, "class");
    if (!classAttr.isEmpty()) {
        StyleAttributes classStyle = parseCssClass(classAttr);
        para.style = classStyle;
    }
    
    // 解析 style 属性
    QString styleAttr = extractAttribute(attrs, "style");
    if (!styleAttr.isEmpty()) {
        StyleAttributes inlineStyle = parseInlineStyle(styleAttr);
        if (inlineStyle.fontSize != 12) para.style.fontSize = inlineStyle.fontSize;
        if (inlineStyle.color != "#000000") para.style.color = inlineStyle.color;
        if (inlineStyle.alignment != "left") para.style.alignment = inlineStyle.alignment;
        para.style.bold = inlineStyle.bold;
        para.style.italic = inlineStyle.italic;
        para.style.underline = inlineStyle.underline;
    }
    
    para.text = stripHtmlTags(content).trimmed();
    if (!para.text.isEmpty()) {
        model.addParagraph(para);
    }
}

void HTMLParser::parseImageTag(const QString &attrs, DocumentModel &model) {
    ImageElement image;
    
    image.src = extractAttribute(attrs, "src");
    
    // 处理相对路径
    if (!m_basePath.isEmpty() && !image.src.startsWith("http") && !QFileInfo(image.src).isAbsolute()) {
        image.src = m_basePath + "/" + image.src;
    }
    
    QString widthAttr = extractAttribute(attrs, "width");
    if (!widthAttr.isEmpty()) {
        image.width = widthAttr.remove("px").toInt();
    }
    
    QString heightAttr = extractAttribute(attrs, "height");
    if (!heightAttr.isEmpty()) {
        image.height = heightAttr.remove("px").toInt();
    }
    
    QString alignAttr = extractAttribute(attrs, "align");
    if (!alignAttr.isEmpty()) {
        image.alignment = alignAttr.toLower();
    }
    
    // 从 style 属性解析对齐
    QString styleAttr = extractAttribute(attrs, "style");
    if (styleAttr.contains("margin") && styleAttr.contains("auto")) {
        image.alignment = "center";
    }
    
    if (!image.src.isEmpty()) {
        model.addImage(image);
    }
}

void HTMLParser::parseTableTag(const QString &content, DocumentModel &model) {
    TableElement table;
    
    // 解析行
    QRegularExpression trRe(R"(<tr([^>]*)>(.*?)</tr>)", 
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator trIt = trRe.globalMatch(content);
    
    QVector<QVector<CellElement>> rows;
    int maxCols = 0;
    
    while (trIt.hasNext()) {
        QRegularExpressionMatch trMatch = trIt.next();
        QString rowContent = trMatch.captured(2);
        
        QVector<CellElement> row;
        
        // 解析单元格 (th 和 td)
        QRegularExpression cellRe(R"(<(th|td)([^>]*)>(.*?)</\1>)", 
            QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatchIterator cellIt = cellRe.globalMatch(rowContent);
        
        while (cellIt.hasNext()) {
            QRegularExpressionMatch cellMatch = cellIt.next();
            CellElement cell;
            
            QString tag = cellMatch.captured(1).toLower();
            QString attrs = cellMatch.captured(2);
            
            // th 默认加粗居中
            if (tag == "th") {
                cell.style.bold = true;
                cell.alignment = "center";
            }
            
            // 解析对齐
            QString alignAttr = extractAttribute(attrs, "align");
            if (!alignAttr.isEmpty()) {
                cell.alignment = alignAttr.toLower();
            }
            
            QString valignAttr = extractAttribute(attrs, "valign");
            if (!valignAttr.isEmpty()) {
                cell.valign = valignAttr.toLower();
            }
            
            // 解析合并
            QString colspanAttr = extractAttribute(attrs, "colspan");
            if (!colspanAttr.isEmpty()) {
                cell.colspan = colspanAttr.toInt();
            }
            
            QString rowspanAttr = extractAttribute(attrs, "rowspan");
            if (!rowspanAttr.isEmpty()) {
                cell.rowspan = rowspanAttr.toInt();
            }
            
            cell.content = stripHtmlTags(cellMatch.captured(3)).trimmed();
            row.append(cell);
        }
        
        if (!row.isEmpty()) {
            rows.append(row);
            if (row.size() > maxCols) {
                maxCols = row.size();
            }
        }
    }
    
    if (!rows.isEmpty()) {
        table.rows = rows.size();
        table.cols = maxCols;
        table.widthType = "auto";
        table.cells = rows;
        table.rowHeights.resize(table.rows);
        table.colWidths.resize(table.cols);
        
        model.addTable(table);
    }
}

QString HTMLParser::extractAttribute(const QString &attrs, const QString &name) {
    QRegularExpression re(QString(R"(%1\s*=\s*["']([^"']*)["'])").arg(name), 
        QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(attrs);
    if (match.hasMatch()) {
        return match.captured(1);
    }
    return QString();
}

QString HTMLParser::stripHtmlTags(const QString &html) {
    QString result = html;
    
    // 处理 <br> 标签
    result.replace(QRegularExpression(R"(<br\s*/?>)", QRegularExpression::CaseInsensitiveOption), "\n");
    
    // 移除所有其他标签
    result.replace(QRegularExpression(R"(<[^>]*>)"), "");
    
    // 解码 HTML 实体
    result.replace("&nbsp;", " ");
    result.replace("&lt;", "<");
    result.replace("&gt;", ">");
    result.replace("&amp;", "&");
    result.replace("&quot;", "\"");
    result.replace("&#39;", "'");
    
    return result;
}

QString HTMLParser::colorNameToHex(const QString &colorName) {
    QString color = colorName.trimmed().toLower();
    
    if (color.startsWith("#")) {
        return color;
    }
    
    if (color.startsWith("rgb")) {
        QRegularExpression rgbRe(R"(rgb\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\))");
        QRegularExpressionMatch match = rgbRe.match(color);
        if (match.hasMatch()) {
            int r = match.captured(1).toInt();
            int g = match.captured(2).toInt();
            int b = match.captured(3).toInt();
            return QString("#%1%2%3")
                .arg(r, 2, 16, QChar('0'))
                .arg(g, 2, 16, QChar('0'))
                .arg(b, 2, 16, QChar('0'));
        }
    }
    
    static QMap<QString, QString> colorMap = {
        {"black", "#000000"}, {"white", "#ffffff"}, {"red", "#ff0000"},
        {"green", "#008000"}, {"blue", "#0000ff"}, {"yellow", "#ffff00"},
        {"orange", "#ffa500"}, {"purple", "#800080"}, {"pink", "#ffc0cb"},
        {"gray", "#808080"}, {"grey", "#808080"}, {"silver", "#c0c0c0"},
        {"navy", "#000080"}, {"teal", "#008080"}, {"maroon", "#800000"},
        {"olive", "#808000"}, {"lime", "#00ff00"}, {"aqua", "#00ffff"},
        {"fuchsia", "#ff00ff"}, {"cyan", "#00ffff"}
    };
    
    return colorMap.value(color, "#000000");
}

QString HTMLParser::convertToXml(const QString &htmlContent) {
    auto model = parse(htmlContent);
    if (!model) {
        return QString();
    }
    
    // 构建 XML 文档
    QString xml = R"(<?xml version="1.0" encoding="UTF-8"?>
<document)";
    
    if (!model->title.isEmpty()) {
        xml += QString(" title=\"%1\"").arg(model->title.toHtmlEscaped());
    }
    xml += ">\n";
    
    // 遍历所有元素并序列化
    for (const auto &elem : model->elements) {
        switch (elem.type) {
            case ElementType::Paragraph: {
                const auto &para = elem.paragraph;
                xml += QString("    <paragraph align=\"%1\"").arg(para.style.alignment);
                if (para.style.spacingBefore > 0) {
                    xml += QString(" spacingBefore=\"%1\"").arg(para.style.spacingBefore);
                }
                if (para.style.spacingAfter > 0) {
                    xml += QString(" spacingAfter=\"%1\"").arg(para.style.spacingAfter);
                }
                xml += ">\n";
                xml += QString("        <text font=\"%1\" size=\"%2\" color=\"%3\"")
                    .arg(para.style.fontFamily)
                    .arg(para.style.fontSize)
                    .arg(para.style.color);
                if (para.style.bold) xml += " bold=\"true\"";
                if (para.style.italic) xml += " italic=\"true\"";
                if (para.style.underline) xml += " underline=\"true\"";
                xml += ">\n";
                xml += QString("            %1\n").arg(para.text.toHtmlEscaped());
                xml += "        </text>\n";
                xml += "    </paragraph>\n";
                break;
            }
            
            case ElementType::Image: {
                const auto &img = elem.image;
                xml += QString("    <image src=\"%1\"").arg(img.src.toHtmlEscaped());
                if (img.width > 0) {
                    xml += QString(" width=\"%1\"").arg(img.width);
                }
                if (img.height > 0) {
                    xml += QString(" height=\"%1\"").arg(img.height);
                }
                if (img.alignment != "left") {
                    xml += QString(" align=\"%1\"").arg(img.alignment);
                }
                xml += "/>\n";
                break;
            }
            
            case ElementType::Table: {
                const auto &tbl = elem.table;
                xml += QString("    <table rows=\"%1\" cols=\"%2\" width=\"%3\">\n")
                    .arg(tbl.rows)
                    .arg(tbl.cols)
                    .arg(tbl.widthType == "auto" ? "auto" : QString::number(tbl.width));
                
                for (int i = 0; i < tbl.rows && i < tbl.cells.size(); ++i) {
                    xml += "        <row";
                    if (i < tbl.rowHeights.size() && tbl.rowHeights[i] > 0) {
                        xml += QString(" height=\"%1\"").arg(tbl.rowHeights[i]);
                    }
                    xml += ">\n";
                    
                    for (int j = 0; j < tbl.cols && j < tbl.cells[i].size(); ++j) {
                        const auto &cell = tbl.cells[i][j];
                        xml += QString("            <cell align=\"%1\"").arg(cell.alignment);
                        if (cell.valign != "top") {
                            xml += QString(" valign=\"%1\"").arg(cell.valign);
                        }
                        if (cell.colspan > 1) {
                            xml += QString(" colspan=\"%1\"").arg(cell.colspan);
                        }
                        if (cell.rowspan > 1) {
                            xml += QString(" rowspan=\"%1\"").arg(cell.rowspan);
                        }
                        if (cell.style.bold) {
                            xml += " bold=\"true\"";
                        }
                        xml += QString(">%1</cell>\n").arg(cell.content.toHtmlEscaped());
                    }
                    
                    xml += "        </row>\n";
                }
                
                xml += "    </table>\n";
                break;
            }
        }
    }
    
    xml += "</document>\n";
    return xml;
}
