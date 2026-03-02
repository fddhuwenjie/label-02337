#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <QString>
#include <QMap>
#include <QRegularExpression>
#include <memory>
#include "DocumentModel.h"

/**
 * HTML 解析器
 * 将 HTML 内容转换为内部文档模型
 */
class HTMLParser {
public:
    HTMLParser();
    ~HTMLParser();
    
    // 解析 HTML 字符串
    std::unique_ptr<DocumentModel> parse(const QString &htmlContent);
    
    // 解析 HTML 文件
    std::unique_ptr<DocumentModel> parseFile(const QString &filePath);
    
    // 将 HTML 转换为 XML 格式
    QString convertToXml(const QString &htmlContent);
    
    // 获取错误信息
    QString lastError() const { return m_lastError; }

private:
    // 解析 CSS 样式
    StyleAttributes parseInlineStyle(const QString &styleStr);
    StyleAttributes parseCssClass(const QString &className);
    
    // 解析 HTML 元素
    void parseElement(const QString &tag, const QString &attrs, const QString &content, DocumentModel &model);
    void parseParagraphTag(const QString &attrs, const QString &content, DocumentModel &model);
    void parseHeadingTag(int level, const QString &attrs, const QString &content, DocumentModel &model);
    void parseImageTag(const QString &attrs, DocumentModel &model);
    void parseTableTag(const QString &content, DocumentModel &model);
    
    // 辅助函数
    QString extractAttribute(const QString &attrs, const QString &name);
    QString stripHtmlTags(const QString &html);
    QString colorNameToHex(const QString &colorName);
    
    // 解析富文本内容为 TextRun 列表
    QVector<TextRun> parseRichText(const QString &content, const StyleAttributes &baseStyle);
    
    QString m_lastError;
    QMap<QString, StyleAttributes> m_cssClasses;
    QString m_basePath;  // HTML 文件所在目录，用于解析相对路径
};

#endif // HTMLPARSER_H
