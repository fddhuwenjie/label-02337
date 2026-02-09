#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <memory>
#include "DocumentModel.h"

class XMLParser {
public:
    XMLParser();
    ~XMLParser();
    
    // 解析 XML 字符串，返回文档模型
    std::unique_ptr<DocumentModel> parse(const QString &xmlContent);
    
    // 验证 XML 格式
    bool validate(const QString &xmlContent, QString &errorMessage);
    
    // 将文档模型序列化为 XML
    QString serialize(const DocumentModel &model);
    
    // 获取最后的错误信息
    QString lastError() const { return m_lastError; }

private:
    void parseDocument(QXmlStreamReader &reader, DocumentModel &model);
    void parseParagraph(QXmlStreamReader &reader, ParagraphElement &para);
    void parseText(QXmlStreamReader &reader, ParagraphElement &para);
    void parseImage(QXmlStreamReader &reader, ImageElement &image);
    void parseTable(QXmlStreamReader &reader, TableElement &table);
    void parseRow(QXmlStreamReader &reader, TableElement &table, int rowIndex);
    void parseCell(QXmlStreamReader &reader, CellElement &cell);
    
    StyleAttributes parseStyleAttributes(const QXmlStreamAttributes &attrs);
    QString colorToHex(const QString &color);
    
    void serializeParagraph(QXmlStreamWriter &writer, const ParagraphElement &para);
    void serializeImage(QXmlStreamWriter &writer, const ImageElement &image);
    void serializeTable(QXmlStreamWriter &writer, const TableElement &table);
    void serializeStyleAttributes(QXmlStreamWriter &writer, const StyleAttributes &style);
    
    QString m_lastError;
};

#endif // XMLPARSER_H
