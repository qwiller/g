#include "core/application.h"
#include "core/config_manager.h"
#include "core/logger.h"
#include "rag/rag_engine.h"
#include "ui/main_window.h"

#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QMessageBox>
#include <QSysInfo>
#include <QProcess>

Application* Application::s_instance = nullptr;

Application::Application(int argc, char* argv[])
    : QApplication(argc, argv)
    , m_configManager(std::make_unique<ConfigManager>())
    , m_logger(nullptr)  // Logger是单例，不需要在这里创建
    , m_ragEngine(std::make_unique<RAGEngine>())
    , m_mainWindow(std::make_unique<MainWindow>())
    , m_initialized(false)
{
    s_instance = this;
    
    // 设置应用程序信息
    setApplicationName("银河麒麟智能问答助手");
    setApplicationVersion("1.0.0");
    setOrganizationName("麒麟软件");
    setOrganizationDomain("kylinos.cn");
    
    // 连接退出信号
    connect(this, &QApplication::aboutToQuit, this, &Application::onAboutToQuit);
}

Application::~Application()
{
    s_instance = nullptr;
}

bool Application::initialize()
{
    qDebug() << "开始初始化银河麒麟智能问答助手...";
    
    // 1. 检查麒麟系统兼容性
    if (!checkKylinCompatibility()) {
        QMessageBox::warning(nullptr, "兼容性警告", 
            "当前系统可能不是银河麒麟操作系统，某些功能可能无法正常工作。");
    }
    
    // 2. 初始化配置系统
    if (!initializeConfig()) {
        QMessageBox::critical(nullptr, "初始化失败", "配置系统初始化失败");
        return false;
    }
    
    // 3. 初始化日志系统
    if (!initializeLogging()) {
        QMessageBox::critical(nullptr, "初始化失败", "日志系统初始化失败");
        return false;
    }
    
    LOG_INFO("应用程序开始初始化", "Application");
    
    // 4. 初始化RAG引擎
    if (!initializeRAGEngine()) {
        LOG_ERROR("RAG引擎初始化失败", "Application");
        QMessageBox::critical(nullptr, "初始化失败", "RAG引擎初始化失败");
        return false;
    }
    
    // 5. 初始化用户界面
    if (!initializeUI()) {
        LOG_ERROR("用户界面初始化失败", "Application");
        QMessageBox::critical(nullptr, "初始化失败", "用户界面初始化失败");
        return false;
    }
    
    m_initialized = true;
    LOG_INFO("应用程序初始化完成", "Application");
    
    return true;
}

int Application::run()
{
    if (!m_initialized) {
        qCritical() << "应用程序未初始化";
        return -1;
    }
    
    // 显示主窗口
    m_mainWindow->show();
    
    LOG_INFO("应用程序开始运行", "Application");
    
    // 进入事件循环
    return exec();
}

ConfigManager* Application::getConfigManager() const
{
    return m_configManager.get();
}

Logger* Application::getLogger() const
{
    return Logger::getInstance();
}

RAGEngine* Application::getRAGEngine() const
{
    return m_ragEngine.get();
}

Application* Application::getInstance()
{
    return s_instance;
}

void Application::onAboutToQuit()
{
    LOG_INFO("应用程序准备退出", "Application");
    
    // 保存配置
    if (m_configManager) {
        m_configManager->saveConfig();
    }
    
    // 关闭日志系统
    if (m_logger) {
        m_logger->shutdown();
    }
    
    LOG_INFO("应用程序退出完成", "Application");
}

bool Application::initializeLogging()
{
    m_logger = Logger::getInstance();
    
    // 从配置中读取日志设置
    LogConfig logConfig;
    logConfig.level = static_cast<LogLevel>(m_configManager->getInt("logging.level", 1));
    logConfig.logFile = m_configManager->getString("logging.file", "./logs/kylin_qa.log");
    logConfig.consoleOutput = m_configManager->getBool("logging.console", true);
    logConfig.maxFileSize = m_configManager->getInt("logging.max_size", 10485760);
    logConfig.maxFiles = m_configManager->getInt("logging.max_files", 5);
    
    return m_logger->initialize(logConfig);
}

bool Application::initializeConfig()
{
    // 尝试加载配置文件
    QString configPath = "config/app_config.json";
    
    // 如果配置文件不存在，尝试从系统配置目录加载
    if (!QFile::exists(configPath)) {
        QString systemConfigPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) 
                                 + "/kylin-qa-assistant/app_config.json";
        if (QFile::exists(systemConfigPath)) {
            configPath = systemConfigPath;
        }
    }
    
    if (!m_configManager->loadConfig(configPath)) {
        qWarning() << "无法加载配置文件，使用默认配置";
        // 即使加载失败也继续，使用默认配置
    }
    
    return true;
}

bool Application::initializeRAGEngine()
{
    // 从配置中读取RAG设置
    RAGConfig ragConfig;
    ragConfig.topK = m_configManager->getInt("rag.top_k", 5);
    ragConfig.similarityThreshold = m_configManager->getDouble("rag.similarity_threshold", 0.7);
    ragConfig.maxTokens = m_configManager->getInt("ai.max_tokens", 2048);
    ragConfig.temperature = m_configManager->getDouble("ai.temperature", 0.7);
    ragConfig.useReranking = m_configManager->getBool("rag.use_reranking", true);
    
    return m_ragEngine->initialize(ragConfig);
}

bool Application::initializeUI()
{
    if (!m_mainWindow->initialize(m_ragEngine.get(), m_configManager.get())) {
        return false;
    }
    
    // 应用窗口设置
    int width = m_configManager->getInt("application.window.width", 1200);
    int height = m_configManager->getInt("application.window.height", 800);
    m_mainWindow->resize(width, height);
    
    return true;
}

bool Application::checkKylinCompatibility()
{
    // 检查操作系统信息
    QString osName = QSysInfo::productType();
    QString osVersion = QSysInfo::productVersion();
    
    LOG_INFO(QString("检测到操作系统: %1 %2").arg(osName, osVersion), "Application");
    
    // 检查是否为麒麟系统
    bool isKylin = false;
    
    // 方法1: 检查系统名称
    if (osName.contains("kylin", Qt::CaseInsensitive) || 
        osName.contains("neokylin", Qt::CaseInsensitive)) {
        isKylin = true;
    }
    
    // 方法2: 检查发行版信息文件
    if (!isKylin) {
        QFile releaseFile("/etc/os-release");
        if (releaseFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&releaseFile);
            QString content = in.readAll();
            if (content.contains("Kylin", Qt::CaseInsensitive)) {
                isKylin = true;
            }
        }
    }
    
    // 方法3: 检查特定的麒麟系统文件
    if (!isKylin) {
        QStringList kylinFiles = {
            "/etc/kylin-release",
            "/usr/bin/kylin-system-updater",
            "/usr/share/kylin-nm"
        };
        
        for (const QString& file : kylinFiles) {
            if (QFile::exists(file)) {
                isKylin = true;
                break;
            }
        }
    }
    
    if (isKylin) {
        LOG_INFO("确认运行在银河麒麟操作系统上", "Application");
    } else {
        LOG_WARNING("未检测到银河麒麟操作系统", "Application");
    }
    
    // 检查CPU架构
    QString architecture = QSysInfo::currentCpuArchitecture();
    LOG_INFO(QString("CPU架构: %1").arg(architecture), "Application");
    
    // 支持的架构: x86_64, aarch64, mips64
    QStringList supportedArchs = {"x86_64", "aarch64", "mips64"};
    bool archSupported = supportedArchs.contains(architecture);
    
    if (!archSupported) {
        LOG_WARNING(QString("CPU架构 %1 可能不被完全支持").arg(architecture), "Application");
    }
    
    return isKylin && archSupported;
}
