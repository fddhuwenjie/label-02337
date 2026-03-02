#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <memory>

/**
 * 日志记录器
 * 支持文件日志和控制台输出
 */
class Logger {
public:
    enum Level {
        Debug = 0,
        Info,
        Warning,
        Error,
        Fatal
    };
    
    static Logger& instance();
    
    // 设置日志文件
    void setLogFile(const QString &filePath);
    
    // 设置最小日志级别
    void setMinLevel(Level level) { m_minLevel = level; }
    
    // 启用/禁用控制台输出
    void setConsoleOutput(bool enabled) { m_consoleOutput = enabled; }
    
    // 日志方法
    void debug(const QString &message, const QString &source = QString());
    void info(const QString &message, const QString &source = QString());
    void warning(const QString &message, const QString &source = QString());
    void error(const QString &message, const QString &source = QString());
    void fatal(const QString &message, const QString &source = QString());
    
    // 通用日志方法
    void log(Level level, const QString &message, const QString &source = QString());

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    QString levelToString(Level level);
    void writeLog(const QString &formattedMessage);
    
    QFile m_logFile;
    QTextStream m_stream;
    QMutex m_mutex;
    Level m_minLevel;
    bool m_consoleOutput;
};

// 便捷宏
#define LOG_DEBUG(msg) Logger::instance().debug(msg, __FUNCTION__)
#define LOG_INFO(msg) Logger::instance().info(msg, __FUNCTION__)
#define LOG_WARNING(msg) Logger::instance().warning(msg, __FUNCTION__)
#define LOG_ERROR(msg) Logger::instance().error(msg, __FUNCTION__)
#define LOG_FATAL(msg) Logger::instance().fatal(msg, __FUNCTION__)

#endif // LOGGER_H
