#include "core/logger.h"

#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <QMutexLocker>
#include <QStandardPaths>
#include <iostream>

Logger* Logger::s_instance = nullptr;
QMutex Logger::s_mutex;

Logger::Logger(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_currentFileSize(0)
    , m_debugCount(0)
    , m_infoCount(0)
    , m_warningCount(0)
    , m_errorCount(0)
    , m_criticalCount(0)
{
}

Logger::~Logger()
{
    shutdown();
}

Logger* Logger::getInstance()
{
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new Logger();
    }
    return s_instance;
}

bool Logger::initialize(const LogConfig& config)
{
    QMutexLocker locker(&m_writeMutex);
    
    m_config = config;
    
    if (m_config.fileOutput) {
        if (!createLogDirectory(m_config.logFile)) {
            return false;
        }
        
        m_logFile = std::make_unique<QFile>(m_config.logFile);
        
        if (!m_logFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
            qCritical() << "无法打开日志文件:" << m_config.logFile;
            return false;
        }
        
        m_logStream = std::make_unique<QTextStream>(m_logFile.get());
        m_logStream->setCodec("UTF-8");
        
        // 获取当前文件大小
        m_currentFileSize = m_logFile->size();
    }
    
    m_initialized = true;
    
    // 记录初始化信息
    info("日志系统初始化完成", "Logger");
    info(QString("日志级别: %1").arg(levelToString(m_config.level)), "Logger");
    info(QString("日志文件: %1").arg(m_config.logFile), "Logger");
    
    return true;
}

void Logger::debug(const QString& message, const QString& category)
{
    log(LogLevel::Debug, message, category);
}

void Logger::info(const QString& message, const QString& category)
{
    log(LogLevel::Info, message, category);
}

void Logger::warning(const QString& message, const QString& category)
{
    log(LogLevel::Warning, message, category);
}

void Logger::error(const QString& message, const QString& category)
{
    log(LogLevel::Error, message, category);
}

void Logger::critical(const QString& message, const QString& category)
{
    log(LogLevel::Critical, message, category);
}

void Logger::log(LogLevel level, const QString& message, const QString& category)
{
    if (!m_initialized || level < m_config.level) {
        return;
    }
    
    QMutexLocker locker(&m_writeMutex);
    
    // 更新统计
    switch (level) {
    case LogLevel::Debug:
        m_debugCount++;
        break;
    case LogLevel::Info:
        m_infoCount++;
        break;
    case LogLevel::Warning:
        m_warningCount++;
        break;
    case LogLevel::Error:
        m_errorCount++;
        break;
    case LogLevel::Critical:
        m_criticalCount++;
        break;
    }
    
    QString formattedMessage = formatMessage(level, message, category);
    QString timestamp = getCurrentTimestamp();
    
    // 输出到控制台
    if (m_config.consoleOutput) {
        writeToConsole(formattedMessage, level);
    }
    
    // 输出到文件
    if (m_config.fileOutput && m_logStream) {
        writeToFile(formattedMessage);
    }
    
    // 发送信号
    emit logMessage(level, message, category, timestamp);
}

void Logger::setLogLevel(LogLevel level)
{
    m_config.level = level;
}

LogLevel Logger::getLogLevel() const
{
    return m_config.level;
}

void Logger::setConsoleOutput(bool enabled)
{
    m_config.consoleOutput = enabled;
}

void Logger::setFileOutput(bool enabled)
{
    m_config.fileOutput = enabled;
}

void Logger::flush()
{
    QMutexLocker locker(&m_writeMutex);
    if (m_logStream) {
        m_logStream->flush();
    }
    if (m_logFile) {
        m_logFile->flush();
    }
}

void Logger::shutdown()
{
    if (m_initialized) {
        info("日志系统关闭", "Logger");
        flush();
        
        if (m_logStream) {
            m_logStream.reset();
        }
        if (m_logFile) {
            m_logFile->close();
            m_logFile.reset();
        }
        
        m_initialized = false;
    }
}

QString Logger::getLogFilePath() const
{
    return m_config.logFile;
}

QJsonObject Logger::getLogStats() const
{
    QJsonObject stats;
    stats["debug_count"] = m_debugCount;
    stats["info_count"] = m_infoCount;
    stats["warning_count"] = m_warningCount;
    stats["error_count"] = m_errorCount;
    stats["critical_count"] = m_criticalCount;
    stats["total_count"] = m_debugCount + m_infoCount + m_warningCount + m_errorCount + m_criticalCount;
    stats["log_file"] = m_config.logFile;
    stats["file_size"] = static_cast<int>(m_currentFileSize);
    return stats;
}

QString Logger::formatMessage(LogLevel level, const QString& message, const QString& category)
{
    QString timestamp = getCurrentTimestamp();
    QString levelStr = levelToString(level);
    QString categoryStr = category.isEmpty() ? "General" : category;
    
    return QString("[%1] [%2] [%3] %4")
           .arg(timestamp)
           .arg(levelStr)
           .arg(categoryStr)
           .arg(message);
}

void Logger::writeToFile(const QString& formattedMessage)
{
    if (!m_logStream) {
        return;
    }
    
    *m_logStream << formattedMessage << Qt::endl;
    m_currentFileSize += formattedMessage.toUtf8().size() + 1;
    
    // 检查是否需要轮转日志文件
    if (m_currentFileSize > m_config.maxFileSize) {
        rotateLogFile();
    }
}

void Logger::writeToConsole(const QString& formattedMessage, LogLevel level)
{
    switch (level) {
    case LogLevel::Debug:
    case LogLevel::Info:
        std::cout << formattedMessage.toStdString() << std::endl;
        break;
    case LogLevel::Warning:
    case LogLevel::Error:
    case LogLevel::Critical:
        std::cerr << formattedMessage.toStdString() << std::endl;
        break;
    }
}

void Logger::rotateLogFile()
{
    if (!m_logFile) {
        return;
    }
    
    QString baseName = m_config.logFile;
    
    // 关闭当前文件
    m_logStream.reset();
    m_logFile->close();
    
    // 轮转现有文件
    for (int i = m_config.maxFiles - 1; i > 0; --i) {
        QString oldName = QString("%1.%2").arg(baseName).arg(i);
        QString newName = QString("%1.%2").arg(baseName).arg(i + 1);
        
        if (QFile::exists(oldName)) {
            QFile::remove(newName);
            QFile::rename(oldName, newName);
        }
    }
    
    // 重命名当前文件
    QString backupName = QString("%1.1").arg(baseName);
    QFile::remove(backupName);
    QFile::rename(baseName, backupName);
    
    // 重新打开新文件
    if (m_logFile->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        m_logStream = std::make_unique<QTextStream>(m_logFile.get());
        m_logStream->setCodec("UTF-8");
        m_currentFileSize = 0;
    }
}

bool Logger::createLogDirectory(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();
    
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qCritical() << "无法创建日志目录:" << dir.absolutePath();
            return false;
        }
    }
    
    return true;
}

QString Logger::levelToString(LogLevel level) const
{
    switch (level) {
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warning:
        return "WARN";
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Critical:
        return "CRITICAL";
    default:
        return "UNKNOWN";
    }
}

QString Logger::getCurrentTimestamp() const
{
    return QDateTime::currentDateTime().toString(m_config.dateTimeFormat);
}
