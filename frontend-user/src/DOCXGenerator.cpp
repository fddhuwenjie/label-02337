#include "DOCXGenerator.h"
#include <QFile>
#include <QTextStream>
#include <QTemporaryDir>
#include <QProcess>
#include <QDebug>
#include <zip.h>

DOCXGenerator::DOCXGenerator() : m_imageCount(0) {}

DOCXGenerator::~DOCXGenerator() {}

bool DOCXGenerator::generate(const DocumentModel &model, const QString &outputPath) {
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        m_lastError = "无法创建临时目录";
        return false;
    }
    
    QString tempPath = tempDir.path();
    m_imageCount = 0;
    m_imageRelations.clear();
    
    if (!createDocxStructure(tempPath)) return false;
    if (!writeContentTypes(tempPath)) return false;
    if (!writeRelationships(tempPath)) return false;
    if (!writeStyles(tempPath)) return false;
    if (!writeDocument(tempPath, model)) return false;
    if (!writeDocumentRelationships(tempPath)) return false;
    if (!packageDocx(tempPath, outputPath)) return false;
    
    return true;
}

bool DOCXGenerator::createDocxStructure(const QString &tempDir) {
    QDir dir(tempDir);
    
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

bool DOCXGenerator::writeContentTypes(const QString &tempDir) {
    QString content = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
    <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
    <Default Extension="xml" ContentType="application/xml"/>
    <Default Extension="png" ContentType="image/png"/>
    <Default Extension="jpg" ContentType="image/jpeg"/>
    <Default Extension="jpeg" ContentType="image/jpeg"/>
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


bool DOCXGenerator::writeDocument(const QString &tempDir, const DocumentModel &model) {
    QString bodyContent;
    
    for (const auto &elem : model.elements) {
        switch (elem.type) {
            case ElementType::Paragraph:
                bodyContent += writeParagraph(elem.paragraph);
                break;
            case ElementType::Image:
                m_imageCount++;
                bodyContent += writeImage(elem.image, m_imageCount);
                break;
            case ElementType::Table:
                bodyContent += writeTable(elem.table);
                break;
        }
    }
    
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

QString DOCXGenerator::writeParagraph(const ParagraphElement &para) {
    QString alignment = alignmentToOOXML(para.style.alignment);
    QString runProps = styleToOOXML(para.style);
    
    QString spacing;
    if (para.style.spacingBefore > 0 || para.style.spacingAfter > 0) {
        spacing = QString(R"(<w:spacing w:before="%1" w:after="%2"/>)")
            .arg(para.style.spacingBefore)
            .arg(para.style.spacingAfter);
    }
    
    return QString(R"(
        <w:p>
            <w:pPr>
                %1
                %2
            </w:pPr>
            <w:r>
                <w:rPr>
                    %3
                </w:rPr>
                <w:t xml:space="preserve">%4</w:t>
            </w:r>
        </w:p>)").arg(alignment, spacing, runProps, para.text.toHtmlEscaped());
}

QString DOCXGenerator::writeImage(const ImageElement &image, int imageIndex) {
    QString rId = QString("rId%1").arg(imageIndex + 10);
    m_imageRelations[rId] = QString("media/image%1.png").arg(imageIndex);
    
    // 默认尺寸 (EMU: 1 inch = 914400 EMU)
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

QString DOCXGenerator::writeTable(const TableElement &table) {
    QString rows;
    
    for (int i = 0; i < table.rows && i < table.cells.size(); ++i) {
        QString cells;
        
        for (int j = 0; j < table.cols && j < table.cells[i].size(); ++j) {
            const CellElement &cell = table.cells[i][j];
            QString cellAlign = alignmentToOOXML(cell.alignment);
            QString vAlign;
            if (cell.valign == "middle") {
                vAlign = R"(<w:vAlign w:val="center"/>)";
            } else if (cell.valign == "bottom") {
                vAlign = R"(<w:vAlign w:val="bottom"/>)";
            }
            
            cells += QString(R"(
                <w:tc>
                    <w:tcPr>
                        <w:tcW w:w="0" w:type="auto"/>
                        %1
                    </w:tcPr>
                    <w:p>
                        <w:pPr>%2</w:pPr>
                        <w:r>
                            <w:t>%3</w:t>
                        </w:r>
                    </w:p>
                </w:tc>)").arg(vAlign, cellAlign, cell.content.toHtmlEscaped());
        }
        
        QString rowHeight;
        if (i < table.rowHeights.size() && table.rowHeights[i] > 0) {
            rowHeight = QString(R"(<w:trPr><w:trHeight w:val="%1"/></w:trPr>)")
                .arg(table.rowHeights[i] * 20); // twips
        }
        
        rows += QString(R"(
            <w:tr>
                %1
                %2
            </w:tr>)").arg(rowHeight, cells);
    }
    
    QString tableWidth = table.widthType == "auto" 
        ? R"(<w:tblW w:w="0" w:type="auto"/>)"
        : QString(R"(<w:tblW w:w="%1" w:type="dxa"/>)").arg(table.width * 20);
    
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
            %2
        </w:tbl>)").arg(tableWidth, rows);
}

QString DOCXGenerator::styleToOOXML(const StyleAttributes &style) {
    QString result;
    
    if (style.fontFamily != "Arial") {
        result += QString(R"(<w:rFonts w:ascii="%1" w:hAnsi="%1"/>)").arg(style.fontFamily);
    }
    
    if (style.fontSize != 12) {
        int halfPoints = style.fontSize * 2;
        result += QString(R"(<w:sz w:val="%1"/><w:szCs w:val="%1"/>)").arg(halfPoints);
    }
    
    if (style.color != "#000000") {
        result += colorToOOXML(style.color);
    }
    
    if (style.bold) {
        result += R"(<w:b/>)";
    }
    
    if (style.italic) {
        result += R"(<w:i/>)";
    }
    
    if (style.underline) {
        result += R"(<w:u w:val="single"/>)";
    }
    
    return result;
}

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

QString DOCXGenerator::colorToOOXML(const QString &color) {
    QString hex = color;
    if (hex.startsWith("#")) {
        hex = hex.mid(1);
    }
    return QString(R"(<w:color w:val="%1"/>)").arg(hex);
}

bool DOCXGenerator::packageDocx(const QString &tempDir, const QString &outputPath) {
    int error = 0;
    zip_t *archive = zip_open(outputPath.toUtf8().constData(), ZIP_CREATE | ZIP_TRUNCATE, &error);
    
    if (!archive) {
        m_lastError = QString("无法创建 DOCX 文件: 错误码 %1").arg(error);
        return false;
    }
    
    // 递归添加文件到 ZIP
    QDir dir(tempDir);
    QStringList files = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    
    std::function<bool(const QString&, const QString&)> addToZip = [&](const QString &basePath, const QString &relativePath) -> bool {
        QDir currentDir(basePath);
        QStringList entries = currentDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        
        for (const QString &entry : entries) {
            QString fullPath = basePath + "/" + entry;
            QString zipPath = relativePath.isEmpty() ? entry : relativePath + "/" + entry;
            
            QFileInfo info(fullPath);
            if (info.isDir()) {
                if (!addToZip(fullPath, zipPath)) {
                    return false;
                }
            } else {
                QFile file(fullPath);
                if (!file.open(QIODevice::ReadOnly)) {
                    continue;
                }
                
                QByteArray data = file.readAll();
                file.close();
                
                zip_source_t *source = zip_source_buffer(archive, data.constData(), data.size(), 0);
                if (!source) {
                    m_lastError = "无法创建 ZIP 源";
                    zip_close(archive);
                    return false;
                }
                
                // 需要复制数据因为 zip_source_buffer 不会复制
                char *dataCopy = new char[data.size()];
                memcpy(dataCopy, data.constData(), data.size());
                source = zip_source_buffer(archive, dataCopy, data.size(), 1);
                
                if (zip_file_add(archive, zipPath.toUtf8().constData(), source, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8) < 0) {
                    zip_source_free(source);
                    m_lastError = QString("无法添加文件到 ZIP: %1").arg(zipPath);
                    zip_close(archive);
                    return false;
                }
            }
        }
        return true;
    };
    
    if (!addToZip(tempDir, "")) {
        return false;
    }
    
    if (zip_close(archive) < 0) {
        m_lastError = "无法关闭 ZIP 文件";
        return false;
    }
    
    return true;
}
