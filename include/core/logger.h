#pragma once

#include <QString>
#include <QObject>
#include <QMutex>
#include <QTextStream>
#include <QFile>
#include <memory>

/**
 * @brief 日志级别枚举
 */
enum class LogLevel
{
    Debug = 0,      ///< 调试信息
    Info = 1,       ///< 一般信息
    Warning = 2,    ///< 警告信息
    Error = 3,      ///< 错误信息
    Critical = 4    ///< 严重错误
};

/**
 * @brief 日志配置结构
 */
struct LogConfig
{
    LogLevel level;             ///< 日志级别
    QString logFile;            ///< 日志文件路径
    bool consoleOutput;         ///< 是否输出到控制台
    bool fileOutput;            ///< 是否输出到文件
    int maxFileSize;            ///< 最大文件大小（字节）
    int maxFiles;               ///< 最大文件数量
    QString dateTimeFormat;     ///< 时间格式
    
    LogConfig()
        : level(LogLevel::Info)
        , logFile("./logs/kylin_qa.log")
        , consoleOutput(true)
        , fileOutput(true)
        , maxFileSize(10 * 1024 * 1024)  // 10MB
        , maxFiles(5)
        , dateTimeFormat("yyyy-MM-dd hh:mm:ss.zzz")
    {}
};

/**
 * @brief 日志管理器类
 * 
 * 提供统一的日志记录功能，支持多级别、多输出目标
 */
class Logger : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 获取日志管理器单例
     * @return 日志管理器实例
     */
    static Logger* getInstance();

    /**
     * @brief 初始化日志系统
     * @param config 日志配置
     * @return 初始化是否成功
     */
    bool initialize(const LogConfig& config);

    /**
     * @brief 记录调试信息
     * @param message 日志消息
     * @param category 日志分类
     */
    void debug(const QString& message, const QString& category = "");

    /**
     * @brief 记录一般信息
     * @param message 日志消息
     * @param category 日志分类
     */
    void info(const QString& message, const QString& category = "");

    /**
     * @brief 记录警告信息
     * @param message 日志消息
     * @param category 日志分类
     */
    void warning(const QString& message, const QString& category = "");

    /**
     * @brief 记录错误信息
     * @param message 日志消息
     * @param category 日志分类
     */
    void error(const QString& message, const QString& category = "");

    /**
     * @brief 记录严重错误信息
     * @param message 日志消息
     * @param category 日志分类
     */
    void critical(const QString& message, const QString& category = "");

    /**
     * @brief 记录指定级别的日志
     * @param level 日志级别
     * @param message 日志消息
     * @param category 日志分类
     */
    void log(LogLevel level, const QString& message, const QString& category = "");

    /**
     * @brief 设置日志级别
     * @param level 日志级别
     */
    void setLogLevel(LogLevel level);

    /**
     * @brief 获取当前日志级别
     * @return 日志级别
     */
    LogLevel getLogLevel() const;

    /**
     * @brief 启用/禁用控制台输出
     * @param enabled 是否启用
     */
    void setConsoleOutput(bool enabled);

    /**
     * @brief 启用/禁用文件输出
     * @param enabled 是否启用
     */
    void setFileOutput(bool enabled);

    /**
     * @brief 刷新日志缓冲区
     */
    void flush();

    /**
     * @brief 关闭日志系统
     */
    void shutdown();

    /**
     * @brief 获取日志文件路径
     * @return 日志文件路径
     */
    QString getLogFilePath() const;

    /**
     * @brief 获取日志统计信息
     * @return 统计信息
     */
    QJsonObject getLogStats() const;

signals:
    /**
     * @brief 日志消息信号
     * @param level 日志级别
     * @param message 日志消息
     * @param category 日志分类
     * @param timestamp 时间戳
     */
    void logMessage(LogLevel level, const QString& message, 
                   const QString& category, const QString& timestamp);

private:
    /**
     * @brief 私有构造函数（单例模式）
     */
    explicit Logger(QObject* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~Logger();

    /**
     * @brief 禁用拷贝构造函数
     */
    Logger(const Logger&) = delete;

    /**
     * @brief 禁用赋值操作符
     */
    Logger& operator=(const Logger&) = delete;

    /**
     * @brief 格式化日志消息
     * @param level 日志级别
     * @param message 原始消息
     * @param category 日志分类
     * @return 格式化后的消息
     */
    QString formatMessage(LogLevel level, const QString& message, const QString& category);

    /**
     * @brief 写入日志到文件
     * @param formattedMessage 格式化的消息
     */
    void writeToFile(const QString& formattedMessage);

    /**
     * @brief 写入日志到控制台
     * @param formattedMessage 格式化的消息
     * @param level 日志级别
     */
    void writeToConsole(const QString& formattedMessage, LogLevel level);

    /**
     * @brief 检查并轮转日志文件
     */
    void rotateLogFile();

    /**
     * @brief 创建日志目录
     * @param filePath 日志文件路径
     * @return 创建是否成功
     */
    bool createLogDirectory(const QString& filePath);

    /**
     * @brief 日志级别转字符串
     * @param level 日志级别
     * @return 级别字符串
     */
    QString levelToString(LogLevel level) const;

    /**
     * @brief 获取当前时间戳
     * @return 时间戳字符串
     */
    QString getCurrentTimestamp() const;

private:
    static Logger* s_instance;
    static QMutex s_mutex;
    
    LogConfig m_config;
    std::unique_ptr<QFile> m_logFile;
    std::unique_ptr<QTextStream> m_logStream;
    QMutex m_writeMutex;
    
    bool m_initialized;
    qint64 m_currentFileSize;
    
    // 统计信息
    int m_debugCount;
    int m_infoCount;
    int m_warningCount;
    int m_errorCount;
    int m_criticalCount;
};

// 便利宏定义
#define LOG_DEBUG(msg, category) Logger::getInstance()->debug(msg, category)
#define LOG_INFO(msg, category) Logger::getInstance()->info(msg, category)
#define LOG_WARNING(msg, category) Logger::getInstance()->warning(msg, category)
#define LOG_ERROR(msg, category) Logger::getInstance()->error(msg, category)
#define LOG_CRITICAL(msg, category) Logger::getInstance()->critical(msg, category)
