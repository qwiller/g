#include "core/application.h"
#include "core/logger.h"

#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>
#include <QThread>
#include <QStorageInfo>

/**
 * @brief 设置应用程序路径
 */
void setupApplicationPaths()
{
    // 设置工作目录为应用程序所在目录
    QString appDir = QCoreApplication::applicationDirPath();
    QDir::setCurrent(appDir);
    
    // 创建必要的目录
    QStringList directories = {
        "logs",
        "data",
        "data/vector_db",
        "temp",
        "config"
    };
    
    for (const QString& dir : directories) {
        QDir().mkpath(dir);
    }
}

/**
 * @brief 显示启动画面
 * @param app 应用程序实例
 * @return 启动画面指针
 */
QSplashScreen* showSplashScreen(QApplication* app)
{
    // 创建启动画面
    QPixmap splashPixmap(400, 300);
    splashPixmap.fill(QColor(41, 128, 185)); // 麒麟蓝色
    
    QSplashScreen* splash = new QSplashScreen(splashPixmap);
    splash->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::SplashScreen);
    
    // 显示启动信息
    splash->show();
    splash->showMessage("银河麒麟智能问答助手", 
                       Qt::AlignCenter | Qt::AlignBottom, 
                       Qt::white);
    
    app->processEvents();
    
    return splash;
}

/**
 * @brief 检查系统要求
 * @return 是否满足要求
 */
bool checkSystemRequirements()
{
    // 检查Qt版本
    QString qtVersion = qVersion();
    qDebug() << "Qt版本:" << qtVersion;
    
    // 检查可用内存（简单检查）
    // 在实际应用中，可以使用更精确的方法检查系统资源
    
    // 检查磁盘空间
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QStorageInfo storage(tempPath);
    
    if (storage.isValid()) {
        qint64 availableBytes = storage.bytesAvailable();
        qDebug() << "可用磁盘空间:" << availableBytes / (1024 * 1024) << "MB";
        
        // 至少需要100MB可用空间
        if (availableBytes < 100 * 1024 * 1024) {
            QMessageBox::warning(nullptr, "系统要求", 
                "磁盘空间不足，至少需要100MB可用空间。");
            return false;
        }
    }
    
    return true;
}

/**
 * @brief 处理未捕获的异常
 * @param type 消息类型
 * @param context 上下文
 * @param msg 消息内容
 */
void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    // 获取日志管理器实例
    Logger* logger = Logger::getInstance();
    
    if (!logger) {
        // 如果日志系统还未初始化，输出到标准错误
        fprintf(stderr, "%s\n", msg.toLocal8Bit().constData());
        return;
    }
    
    // 根据消息类型记录日志
    QString category = context.category ? context.category : "Qt";
    
    switch (type) {
    case QtDebugMsg:
        logger->debug(msg, category);
        break;
    case QtInfoMsg:
        logger->info(msg, category);
        break;
    case QtWarningMsg:
        logger->warning(msg, category);
        break;
    case QtCriticalMsg:
        logger->error(msg, category);
        break;
    case QtFatalMsg:
        logger->critical(msg, category);
        break;
    }
}

/**
 * @brief 主函数
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return 程序退出码
 */
int main(int argc, char* argv[])
{
    // 创建应用程序实例
    Application app(argc, argv);
    
    // 设置应用程序属性
    app.setAttribute(Qt::AA_EnableHighDpiScaling);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    // 设置应用程序路径
    setupApplicationPaths();
    
    // 检查系统要求
    if (!checkSystemRequirements()) {
        return -1;
    }
    
    // 显示启动画面
    QSplashScreen* splash = showSplashScreen(&app);
    
    try {
        // 更新启动画面消息
        splash->showMessage("正在初始化配置系统...", 
                           Qt::AlignCenter | Qt::AlignBottom, Qt::white);
        app.processEvents();
        QThread::msleep(500);
        
        splash->showMessage("正在初始化日志系统...", 
                           Qt::AlignCenter | Qt::AlignBottom, Qt::white);
        app.processEvents();
        QThread::msleep(500);
        
        // 初始化应用程序
        if (!app.initialize()) {
            splash->close();
            delete splash;
            QMessageBox::critical(nullptr, "初始化失败", 
                "应用程序初始化失败，请检查系统环境和配置文件。");
            return -1;
        }
        
        splash->showMessage("正在加载用户界面...", 
                           Qt::AlignCenter | Qt::AlignBottom, Qt::white);
        app.processEvents();
        QThread::msleep(500);
        
        // 安装消息处理器
        qInstallMessageHandler(messageHandler);
        
        splash->showMessage("启动完成", 
                           Qt::AlignCenter | Qt::AlignBottom, Qt::white);
        app.processEvents();
        QThread::msleep(1000);
        
        // 关闭启动画面
        splash->finish(nullptr);
        delete splash;
        
        // 运行应用程序
        return app.run();
        
    } catch (const std::exception& e) {
        splash->close();
        delete splash;
        
        QString errorMsg = QString("应用程序发生异常: %1").arg(e.what());
        qCritical() << errorMsg;
        
        QMessageBox::critical(nullptr, "严重错误", errorMsg);
        return -1;
        
    } catch (...) {
        splash->close();
        delete splash;
        
        qCritical() << "应用程序发生未知异常";
        QMessageBox::critical(nullptr, "严重错误", "应用程序发生未知异常");
        return -1;
    }
}
