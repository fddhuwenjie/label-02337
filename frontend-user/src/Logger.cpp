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
