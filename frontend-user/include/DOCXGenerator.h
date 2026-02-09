#ifndef DOCXGENERATOR_H
#define DOCXGENERATOR_H

#include <QString>
#include <QMap>
#include <QDir>
#include "DocumentModel.h"

class DOCXGenerator {
public:
    DOCXGenerator();
    ~DOCXGenerator();
    
    // 生成 DOCX 文件
    bool generate(const DocumentModel &model, const QString &outputPath);
    
    // 获取错误信息
    QString lastError() const { return m_lastError; }

private:
    bool createDocxStructure(const QString &tempDir);
    bool writeContentTypes(const QString &tempDir);
    bool writeDocument(const QString &tempDir, const DocumentModel &model);
    bool writeStyles(const QString &tempDir);
    bool writeRelationships(const QString &tempDir);
    bool writeDocumentRelationships(const QString &tempDir);
    bool packageDocx(const QString &tempDir, const QString &outputPath);
    
    QString writeParagraph(const ParagraphElement &para);
    QString writeImage(const ImageElement &image, int imageIndex);
    QString writeTable(const TableElement &table);
    QString styleToOOXML(const StyleAttributes &style);
    QString alignmentToOOXML(const QString &alignment);
    QString colorToOOXML(const QString &color);
    
    QString m_lastError;
    QMap<QString, QString> m_imageRelations;
    int m_imageCount;
};

#endif // DOCXGENERATOR_H
