/**
 * @file DOCXGenerator.cpp
 * @brief DOCX 文档生成器实现
 * 
 * 本文件实现了将 DocumentModel 转换为 OOXML 格式的 DOCX 文件。
 * 使用 libzip 库进行 ZIP 打包。
 * 
 * @author Developer
 * @date 2024
 */

#include "DOCXGenerator.h"
#include <QFile>
#include <QTextStream>
#include <QTemporaryDir>
#include <QFileInfo>
#include <QDebug>
#include <zip.h>

/**
 * @brief 构造函数
 */
DOCXGenerator::DOCXGenerator() : m_imageCount(0) {}

/**
 * @brief 析构函数
 */
DOCXGenerator::~DOCXGenerator() {}

/**
 * @brief 生成 DOCX 文件
 * @param model 文档模型
 * @param outputPath 输出文件路径
 * @return 成功返回 true，失败返回 false
 */
bool DOCXGenerator::generate(const DocumentModel &model, const QString &outputPath) {
    // 创建临时目录用于构建 DOCX 结构
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        m_lastError = "无法创建临时目录";
        return false;
    }
    
    QString tempPath = tempDir.path();
    m_imageCount = 0;
    m_imageRelations.clear();
    m_imagePaths.clear();
    
    // 按顺序创建 DOCX 各部分
    if (!createDocxStructure(tempPath)) return false;
    if (!writeContentTypes(tempPath)) return false;
    if (!writeRelationships(tempPath)) return false;
    if (!writeStyles(tempPath)) return false;
    if (!writeDocument(tempPath, model)) return false;
    if (!copyImages(tempPath)) return false;  // 复制图片文件
    if (!writeDocumentRelationships(tempPath)) return false;
    if (!packageDocx(tempPath, outputPath)) return false;
    
    return true;
}

/**
 * @brief 创建 DOCX 目录结构
 * @param tempDir 临时目录路径
 * @return 成功返回 true
 */
bool DOCXGenerator::createDocxStructure(const QString &tempDir) {
    QDir dir(tempDir);
    
    // 创建必要的子目录
    if (!dir.mkpath("_rels")) {
        m_lastError = "无法创建 _rels 目录";
        return false;
    }
    if (!dir.mkpath("word/_rels")) {
        m_lastError = "无法创建 word/_rels 目录";
        return false;
    }
    if (!dir.mkpath("word/media")) {
        m_lastError = "无法创建 word/media 目录";
        return false;
    }
    
    return true;
}

/**
 * @brief 写入 [Content_Types].xml
 * @param tempDir 临时目录路径
 * @return 成功返回 true
 */
bool DOCXGenerator::writeContentTypes(const QString &tempDir) {
    QString content = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
    <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
    <Default Extension="xml" ContentType="application/xml"/>
    <Default Extension="png" ContentType="image/png"/>
    <Default Extension="jpg" ContentType="image/jpeg"/>
    <Default Extension="jpeg" ContentType="image/jpeg"/>
    <Default Extension="gif" ContentType="image/gif"/>
    <Default Extension="bmp" ContentType="image/bmp"/>
    <Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>
    <Override PartName="/word/styles.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml"/>
</Types>)";
    
    QFile file(tempDir + "/[Content_Types].xml");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "无法创建 [Content_Types].xml";
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << content;
    file.close();
    
    return true;
}

/**
 * @brief 写入根关系文件 _rels/.rels
 * @param tempDir 临时目录路径
 * @return 成功返回 true
 */
bool DOCXGenerator::writeRelationships(const QString &tempDir) {
    QString content = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
    <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>
</Relationships>)";
    
    QFile file(tempDir + "/_rels/.rels");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "无法创建 _rels/.rels";
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << content;
    file.close();
    
    return true;
}

/**
 * @brief 写入样式文件 word/styles.xml
 * @param tempDir 临时目录路径
 * @return 成功返回 true
 */
bool DOCXGenerator::writeStyles(const QString &tempDir) {
    QString content = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:styles xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">
    <w:docDefaults>
        <w:rPrDefault>
            <w:rPr>
                <w:rFonts w:ascii="Arial" w:hAnsi="Arial" w:eastAsia="SimSun" w:cs="Arial"/>
                <w:sz w:val="24"/>
                <w:szCs w:val="24"/>
            </w:rPr>
        </w:rPrDefault>
    </w:docDefaults>
</w:styles>)";
    
    QFile file(tempDir + "/word/styles.xml");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "无法创建 word/styles.xml";
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << content;
    file.close();
    
    return true;
}

/**
 * @brief 写入主文档 word/document.xml
 * @param tempDir 临时目录路径
 * @param model 文档模型
 * @return 成功返回 true
 */
bool DOCXGenerator::writeDocument(const QString &tempDir, const DocumentModel &model) {
    QString bodyContent;
    
    // 遍历所有文档元素并生成对应的 OOXML
    for (const auto &elem : model.elements) {
        switch (elem.type) {
            case ElementType::Paragraph:
                bodyContent += writeParagraph(elem.paragraph);
                break;
            case ElementType::Image:
                m_imageCount++;
                bodyContent += writeImage(elem.image, m_imageCount, tempDir);
                break;
            case ElementType::Table:
                bodyContent += writeTable(elem.table);
                break;
        }
    }
    
    // 构建完整的 document.xml
    QString content = QString(R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships"
            xmlns:wp="http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing"
            xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"
            xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture">
    <w:body>
%1
        <w:sectPr>
            <w:pgSz w:w="12240" w:h="15840"/>
            <w:pgMar w:top="1440" w:right="1440" w:bottom="1440" w:left="1440"/>
        </w:sectPr>
    </w:body>
</w:document>)").arg(bodyContent);
    
    QFile file(tempDir + "/word/document.xml");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "无法创建 word/document.xml";
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << content;
    file.close();
    
    return true;
}

/**
 * @brief 复制图片文件到 DOCX 的 media 目录
 * @param tempDir 临时目录路径
 * @return 成功返回 true
 */
bool DOCXGenerator::copyImages(const QString &tempDir) {
    for (auto it = m_imagePaths.begin(); it != m_imagePaths.end(); ++it) {
        QString srcPath = it.value();
        QString destPath = tempDir + "/word/" + it.key();
        
        // 检查源文件是否存在
        if (!QFile::exists(srcPath)) {
            qWarning() << "图片文件不存在:" << srcPath;
            continue;  // 跳过不存在的图片，继续处理其他图片
        }
        
        // 复制图片文件
        if (!QFile::copy(srcPath, destPath)) {
            qWarning() << "无法复制图片:" << srcPath << "->" << destPath;
        }
    }
    return true;
}

/**
 * @brief 写入文档关系文件 word/_rels/document.xml.rels
 * @param tempDir 临时目录路径
 * @return 成功返回 true
 */
bool DOCXGenerator::writeDocumentRelationships(const QString &tempDir) {
    QString relationships = R"(<Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles" Target="styles.xml"/>)";
    
    // 添加图片关系
    for (auto it = m_imageRelations.begin(); it != m_imageRelations.end(); ++it) {
        relationships += QString(R"(
    <Relationship Id="%1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/image" Target="%2"/>)")
            .arg(it.key(), it.value());
    }
    
    QString content = QString(R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
    %1
</Relationships>)").arg(relationships);
    
    QFile file(tempDir + "/word/_rels/document.xml.rels");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "无法创建 word/_rels/document.xml.rels";
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << content;
    file.close();
    
    return true;
}

/**
 * @brief 将 TextRun 转换为 OOXML 运行属性
 * @param run TextRun 对象
 * @return OOXML 字符串
 */
QString textRunToOOXML(const TextRun &run) {
    QString result;
    
    if (run.fontFamily != "Arial") {
        result += QString(R"(<w:rFonts w:ascii="%1" w:hAnsi="%1" w:eastAsia="%1"/>)").arg(run.fontFamily);
    }
    if (run.fontSize != 12) {
        int halfPoints = run.fontSize * 2;
        result += QString(R"(<w:sz w:val="%1"/><w:szCs w:val="%1"/>)").arg(halfPoints);
    }
    if (run.color != "#000000") {
        QString hex = run.color;
        if (hex.startsWith("#")) hex = hex.mid(1);
        result += QString(R"(<w:color w:val="%1"/>)").arg(hex.toUpper());
    }
    if (run.bold) {
        result += R"(<w:b/>)";
    }
    if (run.italic) {
        result += R"(<w:i/>)";
    }
    if (run.underline) {
        result += R"(<w:u w:val="single"/>)";
    }
    
    return result;
}

/**
 * @brief 生成段落的 OOXML
 * @param para 段落元素
 * @return OOXML 字符串
 */
QString DOCXGenerator::writeParagraph(const ParagraphElement &para) {
    QString alignment = alignmentToOOXML(para.style.alignment);
    
    // 生成段落间距属性
    QString spacing;
    if (para.style.spacingBefore > 0 || para.style.spacingAfter > 0) {
        spacing = QString(R"(<w:spacing w:before="%1" w:after="%2"/>)")
            .arg(para.style.spacingBefore)
            .arg(para.style.spacingAfter);
    }
    
    // 生成文本运行
    QString runs;
    if (para.hasRuns()) {
        // 使用富文本 TextRun 列表
        for (const auto &run : para.runs) {
            QString runProps = textRunToOOXML(run);
            runs += QString(R"(
            <w:r>
                <w:rPr>%1</w:rPr>
                <w:t xml:space="preserve">%2</w:t>
            </w:r>)").arg(runProps, run.text.toHtmlEscaped());
        }
    } else {
        // 向后兼容：使用单一样式
        QString runProps = styleToOOXML(para.style);
        runs = QString(R"(
            <w:r>
                <w:rPr>%1</w:rPr>
                <w:t xml:space="preserve">%2</w:t>
            </w:r>)").arg(runProps, para.text.toHtmlEscaped());
    }
    
    return QString(R"(
        <w:p>
            <w:pPr>
                %1
                %2
            </w:pPr>%3
        </w:p>)").arg(alignment, spacing, runs);
}

/**
 * @brief 生成图片的 OOXML 并记录图片路径
 * @param image 图片元素
 * @param imageIndex 图片索引
 * @param tempDir 临时目录路径
 * @return OOXML 字符串
 */
QString DOCXGenerator::writeImage(const ImageElement &image, int imageIndex, const QString &tempDir) {
    // 获取图片文件扩展名
    QFileInfo fileInfo(image.src);
    QString ext = fileInfo.suffix().toLower();
    if (ext.isEmpty()) ext = "png";
    
    // 生成关系 ID 和目标路径
    QString rId = QString("rId%1").arg(imageIndex + 10);
    QString mediaPath = QString("media/image%1.%2").arg(imageIndex).arg(ext);
    
    // 记录图片关系和源路径
    m_imageRelations[rId] = mediaPath;
    m_imagePaths[mediaPath] = image.src;
    
    // 计算图片尺寸 (EMU: 1 像素 = 9525 EMU)
    int width = image.width > 0 ? image.width * 9525 : 3000000;
    int height = image.height > 0 ? image.height * 9525 : 2000000;
    
    QString alignment = alignmentToOOXML(image.alignment);
    
    return QString(R"(
        <w:p>
            <w:pPr>
                %1
            </w:pPr>
            <w:r>
                <w:drawing>
                    <wp:inline distT="0" distB="0" distL="0" distR="0">
                        <wp:extent cx="%2" cy="%3"/>
                        <wp:docPr id="%4" name="Picture %4"/>
                        <a:graphic xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
                            <a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture">
                                <pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture">
                                    <pic:nvPicPr>
                                        <pic:cNvPr id="%4" name="Picture %4"/>
                                        <pic:cNvPicPr/>
                                    </pic:nvPicPr>
                                    <pic:blipFill>
                                        <a:blip r:embed="%5"/>
                                        <a:stretch>
                                            <a:fillRect/>
                                        </a:stretch>
                                    </pic:blipFill>
                                    <pic:spPr>
                                        <a:xfrm>
                                            <a:off x="0" y="0"/>
                                            <a:ext cx="%2" cy="%3"/>
                                        </a:xfrm>
                                        <a:prstGeom prst="rect">
                                            <a:avLst/>
                                        </a:prstGeom>
                                    </pic:spPr>
                                </pic:pic>
                            </a:graphicData>
                        </a:graphic>
                    </wp:inline>
                </w:drawing>
            </w:r>
        </w:p>)").arg(alignment).arg(width).arg(height).arg(imageIndex).arg(rId);
}

/**
 * @brief 生成表格的 OOXML，支持单元格合并
 * @param table 表格元素
 * @return OOXML 字符串
 */
QString DOCXGenerator::writeTable(const TableElement &table) {
    QString rows;
    QString gridCols;
    
    // 生成表格列宽网格
    for (int j = 0; j < table.cols; ++j) {
        int colWidth = 0;
        if (j < table.colWidths.size() && table.colWidths[j] > 0) {
            colWidth = table.colWidths[j] * 20;  // 转换为 twips
        }
        gridCols += QString(R"(<w:gridCol w:w="%1"/>)").arg(colWidth);
    }
    
    // 遍历所有行
    for (int i = 0; i < table.rows && i < table.cells.size(); ++i) {
        QString cells;
        
        // 遍历所有列
        for (int j = 0; j < table.cols && j < table.cells[i].size(); ++j) {
            const CellElement &cell = table.cells[i][j];
            
            // 生成单元格属性
            QString cellProps;
            
            // 单元格宽度
            int cellWidth = 0;
            if (j < table.colWidths.size() && table.colWidths[j] > 0) {
                cellWidth = table.colWidths[j] * 20;  // 转换为 twips
                cellProps += QString(R"(<w:tcW w:w="%1" w:type="dxa"/>)").arg(cellWidth);
            } else {
                cellProps += R"(<w:tcW w:w="0" w:type="auto"/>)";
            }
            
            // 水平合并 (colspan)
            if (cell.colspan > 1) {
                cellProps += QString(R"(<w:gridSpan w:val="%1"/>)").arg(cell.colspan);
            }
            
            // 垂直合并 (rowspan) - 起始单元格
            if (cell.rowspan > 1) {
                cellProps += R"(<w:vMerge w:val="restart"/>)";
            }
            
            // 垂直对齐
            if (cell.valign == "middle") {
                cellProps += R"(<w:vAlign w:val="center"/>)";
            } else if (cell.valign == "bottom") {
                cellProps += R"(<w:vAlign w:val="bottom"/>)";
            }
            
            // 生成单元格内容
            QString cellAlign = alignmentToOOXML(cell.alignment);
            QString cellStyle = styleToOOXML(cell.style);
            
            cells += QString(R"(
                <w:tc>
                    <w:tcPr>
                        %1
                    </w:tcPr>
                    <w:p>
                        <w:pPr>%2</w:pPr>
                        <w:r>
                            <w:rPr>%3</w:rPr>
                            <w:t>%4</w:t>
                        </w:r>
                    </w:p>
                </w:tc>)").arg(cellProps, cellAlign, cellStyle, cell.content.toHtmlEscaped());
        }
        
        // 生成行高属性
        QString rowHeight;
        if (i < table.rowHeights.size() && table.rowHeights[i] > 0) {
            rowHeight = QString(R"(<w:trPr><w:trHeight w:val="%1"/></w:trPr>)")
                .arg(table.rowHeights[i] * 20);  // 转换为 twips
        }
        
        rows += QString(R"(
            <w:tr>
                %1
                %2
            </w:tr>)").arg(rowHeight, cells);
    }
    
    // 生成表格宽度属性
    QString tableWidth = table.widthType == "auto" 
        ? R"(<w:tblW w:w="0" w:type="auto"/>)"
        : QString(R"(<w:tblW w:w="%1" w:type="dxa"/>)").arg(table.width * 20);
    
    // 返回完整的表格 OOXML
    return QString(R"(
        <w:tbl>
            <w:tblPr>
                %1
                <w:tblBorders>
                    <w:top w:val="single" w:sz="4" w:space="0" w:color="000000"/>
                    <w:left w:val="single" w:sz="4" w:space="0" w:color="000000"/>
                    <w:bottom w:val="single" w:sz="4" w:space="0" w:color="000000"/>
                    <w:right w:val="single" w:sz="4" w:space="0" w:color="000000"/>
                    <w:insideH w:val="single" w:sz="4" w:space="0" w:color="000000"/>
                    <w:insideV w:val="single" w:sz="4" w:space="0" w:color="000000"/>
                </w:tblBorders>
            </w:tblPr>
            <w:tblGrid>
                %3
            </w:tblGrid>
            %2
        </w:tbl>)").arg(tableWidth, rows, gridCols);
}

/**
 * @brief 将样式属性转换为 OOXML 运行属性
 * @param style 样式属性
 * @return OOXML 字符串
 */
QString DOCXGenerator::styleToOOXML(const StyleAttributes &style) {
    QString result;
    
    // 字体
    if (style.fontFamily != "Arial") {
        result += QString(R"(<w:rFonts w:ascii="%1" w:hAnsi="%1" w:eastAsia="%1"/>)").arg(style.fontFamily);
    }
    
    // 字号 (OOXML 使用半磅为单位)
    if (style.fontSize != 12) {
        int halfPoints = style.fontSize * 2;
        result += QString(R"(<w:sz w:val="%1"/><w:szCs w:val="%1"/>)").arg(halfPoints);
    }
    
    // 颜色
    if (style.color != "#000000") {
        result += colorToOOXML(style.color);
    }
    
    // 加粗
    if (style.bold) {
        result += R"(<w:b/>)";
    }
    
    // 斜体
    if (style.italic) {
        result += R"(<w:i/>)";
    }
    
    // 下划线
    if (style.underline) {
        result += R"(<w:u w:val="single"/>)";
    }
    
    return result;
}

/**
 * @brief 将对齐方式转换为 OOXML
 * @param alignment 对齐方式字符串
 * @return OOXML 字符串
 */
QString DOCXGenerator::alignmentToOOXML(const QString &alignment) {
    if (alignment == "center") {
        return R"(<w:jc w:val="center"/>)";
    } else if (alignment == "right") {
        return R"(<w:jc w:val="right"/>)";
    } else if (alignment == "justify") {
        return R"(<w:jc w:val="both"/>)";
    }
    return R"(<w:jc w:val="left"/>)";
}

/**
 * @brief 将颜色值转换为 OOXML
 * @param color 颜色值 (如 #FF0000)
 * @return OOXML 字符串
 */
QString DOCXGenerator::colorToOOXML(const QString &color) {
    QString hex = color;
    if (hex.startsWith("#")) {
        hex = hex.mid(1);
    }
    return QString(R"(<w:color w:val="%1"/>)").arg(hex.toUpper());
}

/**
 * @brief 将临时目录打包为 DOCX 文件
 * 
 * 使用 libzip 库将临时目录中的所有文件打包为 ZIP 格式的 DOCX 文件。
 * 注意：zip_source_buffer 的第四个参数设为 1 表示 libzip 会自动释放内存。
 * 
 * @param tempDir 临时目录路径
 * @param outputPath 输出文件路径
 * @return 成功返回 true
 */
bool DOCXGenerator::packageDocx(const QString &tempDir, const QString &outputPath) {
    int error = 0;
    zip_t *archive = zip_open(outputPath.toUtf8().constData(), ZIP_CREATE | ZIP_TRUNCATE, &error);
    
    if (!archive) {
        m_lastError = QString("无法创建 DOCX 文件: 错误码 %1").arg(error);
        return false;
    }
    
    // 递归添加文件到 ZIP 的 lambda 函数
    std::function<bool(const QString&, const QString&)> addToZip = [&](const QString &basePath, const QString &relativePath) -> bool {
        QDir currentDir(basePath);
        QStringList entries = currentDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        
        for (const QString &entry : entries) {
            QString fullPath = basePath + "/" + entry;
            QString zipPath = relativePath.isEmpty() ? entry : relativePath + "/" + entry;
            
            QFileInfo info(fullPath);
            if (info.isDir()) {
                // 递归处理子目录
                if (!addToZip(fullPath, zipPath)) {
                    return false;
                }
            } else {
                // 读取文件内容
                QFile file(fullPath);
                if (!file.open(QIODevice::ReadOnly)) {
                    qWarning() << "无法读取文件:" << fullPath;
                    continue;
                }
                
                QByteArray data = file.readAll();
                file.close();
                
                if (data.isEmpty()) {
                    continue;
                }
                
                // 分配内存并复制数据
                // 注意：zip_source_buffer 的 freep 参数设为 1，表示 libzip 会在完成后自动释放内存
                // 这样可以避免内存泄漏
                void *dataCopy = malloc(data.size());
                if (!dataCopy) {
                    m_lastError = "内存分配失败";
                    zip_close(archive);
                    return false;
                }
                memcpy(dataCopy, data.constData(), data.size());
                
                // 创建 ZIP 源，freep=1 表示 libzip 负责释放内存
                zip_source_t *source = zip_source_buffer(archive, dataCopy, data.size(), 1);
                if (!source) {
                    free(dataCopy);  // 如果创建失败，手动释放
                    m_lastError = "无法创建 ZIP 源";
                    zip_close(archive);
                    return false;
                }
                
                // 添加文件到 ZIP
                if (zip_file_add(archive, zipPath.toUtf8().constData(), source, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8) < 0) {
                    zip_source_free(source);  // 添加失败时释放源（内存由 source 管理）
                    m_lastError = QString("无法添加文件到 ZIP: %1").arg(zipPath);
                    zip_close(archive);
                    return false;
                }
                // 添加成功后，内存由 libzip 管理，不需要手动释放
            }
        }
        return true;
    };
    
    // 执行打包
    if (!addToZip(tempDir, "")) {
        return false;
    }
    
    // 关闭 ZIP 文件
    if (zip_close(archive) < 0) {
        m_lastError = "无法关闭 ZIP 文件";
        return false;
    }
    
    return true;
}
