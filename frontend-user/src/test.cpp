#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "XMLParser.h"
#include "DOCXGenerator.h"

bool convertXmlToDocx(const QString &xmlPath, const QString &docxPath) {
    QFile file(xmlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开 XML 文件:" << xmlPath;
        return false;
    }

    QString xmlContent = QString::fromUtf8(file.readAll());
    file.close();

    XMLParser parser;
    std::unique_ptr<DocumentModel> model = parser.parse(xmlContent);
    if (!model) {
        qWarning() << "解析 XML 失败:" << xmlPath << parser.lastError();
        return false;
    }

    DOCXGenerator generator;
    if (!generator.generate(*model, docxPath)) {
        qWarning() << "生成 DOCX 失败:" << docxPath << generator.lastError();
        return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    QStringList samples = {
        "sample1_basic.xml",
        "sample2_rich_text.xml",
        "sample3_table.xml",
        "sample4_report.xml",
        "sample5_invoice.xml"
    };

    QString samplesDir = QCoreApplication::applicationDirPath() + "/../samples";
    QString outputDir = QCoreApplication::applicationDirPath() + "/test_output";

    QDir().mkpath(outputDir);

    int passed = 0;
    int failed = 0;

    qDebug() << "开始运行测试...";
    qDebug() << "样本目录:" << samplesDir;
    qDebug() << "输出目录:" << outputDir;
    qDebug() << "";

    for (const QString &sample : samples) {
        QString xmlPath = samplesDir + "/" + sample;
        QString docxPath = outputDir + "/" + sample.replace(".xml", ".docx");

        qDebug() << "测试:" << sample;

        if (!QFile::exists(xmlPath)) {
            qWarning() << "  ✗ XML 文件不存在:" << xmlPath;
            failed++;
            continue;
        }

        if (convertXmlToDocx(xmlPath, docxPath)) {
            if (QFile::exists(docxPath)) {
                QFileInfo info(docxPath);
                if (info.size() > 0) {
                    qDebug() << "  ✓ 成功生成, 文件大小:" << info.size() << "字节";
                    passed++;
                } else {
                    qWarning() << "  ✗ 文件为空";
                    failed++;
                }
            } else {
                qWarning() << "  ✗ 输出文件不存在";
                failed++;
            }
        } else {
            failed++;
        }
        qDebug() << "";
    }

    qDebug() << "测试结果:";
    qDebug() << "  通过:" << passed;
    qDebug() << "  失败:" << failed;
    qDebug() << "  总计:" << samples.size();

    return (failed == 0) ? 0 : 1;
}
