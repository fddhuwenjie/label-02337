#include "Logger.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

Logger::Logger() 
    : m_minLevel(Info)
    , m_consoleOutput(true)
{
    // 默认日志文件位置
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logDir);
    setLogFile(logDir + "/xml2docx.log");
}

Logger::~Logger() {
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
}

void Logger::setLogFile(const QString &filePath) {
    QMutexLocker locker(&m_mutex);
    
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
    
    m_logFile.setFileName(filePath);
    if (m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        m_stream.setDevice(&m_logFile);
        m_stream.setEncoding(QStringConverter::Utf8);
    }
}

void Logger::debug(const QString &message, const QString &source) {
    log(Debug, message, source);
}

void Logger::info(const QString &message, const QString &source) {
    log(Info, message, source);
}

void Logger::warning(const QString &message, const QString &source) {
    log(Warning, message, source);
}

void Logger::error(const QString &message, const QString &source) {
    log(Error, message, source);
}

void Logger::fatal(const QString &message, const QString &source) {
    log(Fatal, message, source);
}

void Logger::log(Level level, const QString &message, const QString &source) {
    if (level < m_minLevel) {
        return;
    }
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr = levelToString(level);
    
    QString formattedMessage;
    if (source.isEmpty()) {
        formattedMessage = QString("[%1] [%2] %3")
            .arg(timestamp, levelStr, message);
    } else {
        formattedMessage = QString("[%1] [%2] [%3] %4")
            .arg(timestamp, levelStr, source, message);
    }
    
    writeLog(formattedMessage);
}

QString Logger::levelToString(Level level) {
    switch (level) {
        case Debug:   return "DEBUG";
        case Info:    return "INFO ";
        case Warning: return "WARN ";
        case Error:   return "ERROR";
        case Fatal:   return "FATAL";
        default:      return "?????";
    }
}

void Logger::writeLog(const QString &formattedMessage) {
    QMutexLocker locker(&m_mutex);
    
    if (m_consoleOutput) {
        qDebug().noquote() << formattedMessage;
    }
    
    if (m_logFile.isOpen()) {
        m_stream << formattedMessage << "\n";
        m_stream.flush();
    }
}

#ifdef TEST_MODE
#include <QCoreApplication>
#include <QFile>
#include "XMLParser.h"
#include "DOCXGenerator.h"
#include "DocumentModel.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    
    if (argc < 3) {
        qWarning() << "Usage:" << argv[0] << "<input.xml> <output.docx>";
        return 1;
    }
    
    QString inputFile = QString::fromLocal8Bit(argv[1]);
    QString outputFile = QString::fromLocal8Bit(argv[2]);
    
    QFile file(inputFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open XML file:" << inputFile;
        return 1;
    }
    QString xmlContent = QString::fromUtf8(file.readAll());
    file.close();
    
    XMLParser parser;
    std::unique_ptr<DocumentModel> model = parser.parse(xmlContent);
    
    if (!model) {
        qWarning() << "Failed to parse XML:" << inputFile << "Error:" << parser.lastError();
        return 1;
    }
    
    DOCXGenerator generator;
    if (!generator.generate(*model, outputFile)) {
        qWarning() << "Failed to generate DOCX:" << outputFile << "Error:" << generator.lastError();
        return 1;
    }
    
    QFileInfo checkFile(outputFile);
    if (checkFile.exists() && checkFile.size() > 0) {
        qDebug() << "Successfully generated DOCX:" << outputFile << "Size:" << checkFile.size() << "bytes";
        return 0;
    } else {
        qWarning() << "Generated file does not exist or is empty";
        return 1;
    }
}
#endif
