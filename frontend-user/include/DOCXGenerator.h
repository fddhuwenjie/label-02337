#ifndef DOCXGENERATOR_H
#define DOCXGENERATOR_H

#include <QString>
#include <QMap>
#include <QDir>
#include "DocumentModel.h"

/**
 * @class DOCXGenerator
 * @brief DOCX 文档生成器
 * 
 * 将 DocumentModel 转换为 OOXML 格式的 DOCX 文件。
 * 支持文本样式、图片嵌入、表格（含单元格合并）等功能。
 */
class DOCXGenerator {
public:
    DOCXGenerator();
    ~DOCXGenerator();
    
    /**
     * @brief 生成 DOCX 文件
     * @param model 文档模型
     * @param outputPath 输出文件路径
     * @return 成功返回 true，失败返回 false
     */
    bool generate(const DocumentModel &model, const QString &outputPath);
    
    /**
     * @brief 获取最后的错误信息
     * @return 错误信息字符串
     */
    QString lastError() const { return m_lastError; }

private:
    // DOCX 结构创建
    bool createDocxStructure(const QString &tempDir);
    bool writeContentTypes(const QString &tempDir);
    bool writeDocument(const QString &tempDir, const DocumentModel &model);
    bool writeStyles(const QString &tempDir);
    bool writeRelationships(const QString &tempDir);
    bool writeDocumentRelationships(const QString &tempDir);
    bool copyImages(const QString &tempDir);
    bool packageDocx(const QString &tempDir, const QString &outputPath);
    
    // 元素生成
    QString writeParagraph(const ParagraphElement &para);
    QString writeImage(const ImageElement &image, int imageIndex, const QString &tempDir);
    QString writeTable(const TableElement &table);
    
    // 样式转换
    QString styleToOOXML(const StyleAttributes &style);
    QString alignmentToOOXML(const QString &alignment);
    QString colorToOOXML(const QString &color);
    
    QString m_lastError;                    ///< 最后的错误信息
    QMap<QString, QString> m_imageRelations; ///< 图片关系映射 (rId -> 相对路径)
    QMap<QString, QString> m_imagePaths;     ///< 图片路径映射 (相对路径 -> 源文件路径)
    int m_imageCount;                        ///< 图片计数器
};

#endif // DOCXGENERATOR_H
