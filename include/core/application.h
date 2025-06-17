#pragma once

#include <QApplication>
#include <QQmlApplicationEngine>
#include <memory>

class ConfigManager;
class Logger;
class RAGEngine;
class MainWindow;

/**
 * @brief 应用程序主类
 * 
 * 负责应用程序的初始化、配置管理和主要组件的协调
 */
class Application : public QApplication
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param argc 命令行参数数量
     * @param argv 命令行参数数组
     */
    Application(int argc, char* argv[]);
    
    /**
     * @brief 析构函数
     */
    ~Application();

    /**
     * @brief 初始化应用程序
     * @return 初始化是否成功
     */
    bool initialize();

    /**
     * @brief 运行应用程序
     * @return 应用程序退出码
     */
    int run();

    /**
     * @brief 获取配置管理器实例
     * @return 配置管理器指针
     */
    ConfigManager* getConfigManager() const;

    /**
     * @brief 获取日志管理器实例
     * @return 日志管理器指针
     */
    Logger* getLogger() const;

    /**
     * @brief 获取RAG引擎实例
     * @return RAG引擎指针
     */
    RAGEngine* getRAGEngine() const;

    /**
     * @brief 获取应用程序单例实例
     * @return 应用程序实例指针
     */
    static Application* getInstance();

private slots:
    /**
     * @brief 处理应用程序退出
     */
    void onAboutToQuit();

private:
    /**
     * @brief 初始化日志系统
     * @return 初始化是否成功
     */
    bool initializeLogging();

    /**
     * @brief 初始化配置系统
     * @return 初始化是否成功
     */
    bool initializeConfig();

    /**
     * @brief 初始化RAG引擎
     * @return 初始化是否成功
     */
    bool initializeRAGEngine();

    /**
     * @brief 初始化用户界面
     * @return 初始化是否成功
     */
    bool initializeUI();

    /**
     * @brief 检查麒麟系统兼容性
     * @return 是否兼容
     */
    bool checkKylinCompatibility();

private:
    static Application* s_instance;
    
    std::unique_ptr<ConfigManager> m_configManager;
    std::unique_ptr<Logger> m_logger;
    std::unique_ptr<RAGEngine> m_ragEngine;
    std::unique_ptr<MainWindow> m_mainWindow;
    
    bool m_initialized;
};
