#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include "XMLParser.h"
#include "DOCXGenerator.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    
    qDebug() << "XML to DOCX Converter - Test Runner";
    qDebug() << "=====================================";
    
    XMLParser parser;
    DOCXGenerator generator;
    
    QStringList sampleFiles = {
        "samples/sample1_basic.xml",
        "samples/sample2_rich_text.xml",
        "samples/sample3_table.xml",
        "samples/sample4_report.xml",
        "samples/sample5_invoice.xml"
    };
    
    QStringList outputFiles = {
        "test_output/sample1_basic.docx",
        "test_output/sample2_rich_text.docx",
        "test_output/sample3_table.docx",
        "test_output/sample4_report.docx",
        "test_output/sample5_invoice.docx"
    };
    
    QDir testDir("test_output");
    if (!testDir.exists()) {
        testDir.mkpath(".");
    }
    
    int successCount = 0;
    int totalCount = sampleFiles.size();
    
    for (int i = 0; i < totalCount; ++i) {
        QString xmlPath = sampleFiles[i];
        QString docxPath = outputFiles[i];
        
        qDebug() << "\nProcessing:" << xmlPath;
        
        QFile xmlFile(xmlPath);
        if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "  Failed to open file:" << xmlPath;
            continue;
        }
        
        QString xmlContent = QString::fromUtf8(xmlFile.readAll());
        xmlFile.close();
        
        QString errorMsg;
        if (!parser.validate(xmlContent, errorMsg)) {
            qWarning() << "  XML validation failed:" << errorMsg;
            continue;
        }
        
        auto model = parser.parse(xmlContent);
        if (!model) {
            qWarning() << "  XML parsing failed:" << parser.lastError();
            continue;
        }
        
        if (generator.generate(*model, docxPath)) {
            qDebug() << "  ✓ Generated:" << docxPath;
            successCount++;
        } else {
            qWarning() << "  ✗ Generation failed:" << generator.lastError();
        }
    }
    
    qDebug() << "\n=====================================";
    qDebug() << "Test Summary:" << successCount << "/" << totalCount << "samples succeeded";
    
    QFileInfoList outputFileInfos;
    for (const QString &path : outputFiles) {
        outputFileInfos.append(QFileInfo(path));
    }
    
    int existingCount = 0;
    for (const QFileInfo &info : outputFileInfos) {
        if (info.exists()) {
            existingCount++;
            qDebug() << "  ✓ File exists:" << info.fileName();
        } else {
            qWarning() << "  ✗ File missing:" << info.fileName();
        }
    }
    
    qDebug() << "\nVerification:" << existingCount << "/" << totalCount << "output files exist";
    
    return (existingCount == totalCount && successCount == totalCount) ? 0 : 1;
}
