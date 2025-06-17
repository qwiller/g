#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QAction>
#include <memory>

class ChatWidget;
class DocumentManagerWidget;
class SettingsDialog;
class VoiceController;
class RAGEngine;
class ConfigManager;

/**
 * @brief 主窗口类
 * 
 * 应用程序的主界面，整合聊天、文档管理和设置功能
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit MainWindow(QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~MainWindow();

    /**
     * @brief 初始化主窗口
     * @param ragEngine RAG引擎实例
     * @param configManager 配置管理器实例
     * @return 初始化是否成功
     */
    bool initialize(RAGEngine* ragEngine, ConfigManager* configManager);

    /**
     * @brief 显示状态消息
     * @param message 消息内容
     * @param timeout 显示时间（毫秒）
     */
    void showStatusMessage(const QString& message, int timeout = 3000);

    /**
     * @brief 设置进度条值
     * @param value 进度值（0-100）
     */
    void setProgressValue(int value);

    /**
     * @brief 显示/隐藏进度条
     * @param visible 是否显示
     */
    void setProgressVisible(bool visible);

protected:
    /**
     * @brief 关闭事件处理
     * @param event 关闭事件
     */
    void closeEvent(QCloseEvent* event) override;

    /**
     * @brief 窗口大小改变事件
     * @param event 大小改变事件
     */
    void resizeEvent(QResizeEvent* event) override;

    /**
     * @brief 键盘按键事件
     * @param event 按键事件
     */
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    /**
     * @brief 新建对话
     */
    void onNewChat();

    /**
     * @brief 打开文档
     */
    void onOpenDocument();

    /**
     * @brief 保存对话
     */
    void onSaveChat();

    /**
     * @brief 导出对话
     */
    void onExportChat();

    /**
     * @brief 显示设置对话框
     */
    void onShowSettings();

    /**
     * @brief 显示关于对话框
     */
    void onShowAbout();

    /**
     * @brief 切换语音输入
     */
    void onToggleVoiceInput();

    /**
     * @brief 切换语音播报
     */
    void onToggleVoiceOutput();

    /**
     * @brief 清空知识库
     */
    void onClearKnowledgeBase();

    /**
     * @brief 刷新知识库统计
     */
    void onRefreshStats();

    /**
     * @brief 处理文档添加完成
     * @param success 是否成功
     * @param message 结果消息
     */
    void onDocumentAdded(bool success, const QString& message);

    /**
     * @brief 处理查询完成
     * @param success 是否成功
     * @param processingTime 处理时间
     */
    void onQueryCompleted(bool success, int processingTime);

    /**
     * @brief 处理语音识别结果
     * @param text 识别的文本
     */
    void onSpeechRecognized(const QString& text);

    /**
     * @brief 处理语音合成完成
     */
    void onSpeechSynthesized();

private:
    /**
     * @brief 创建菜单栏
     */
    void createMenuBar();

    /**
     * @brief 创建工具栏
     */
    void createToolBar();

    /**
     * @brief 创建状态栏
     */
    void createStatusBar();

    /**
     * @brief 创建中央窗口部件
     */
    void createCentralWidget();

    /**
     * @brief 创建动作
     */
    void createActions();

    /**
     * @brief 连接信号和槽
     */
    void connectSignals();

    /**
     * @brief 应用主题样式
     */
    void applyTheme();

    /**
     * @brief 加载窗口设置
     */
    void loadWindowSettings();

    /**
     * @brief 保存窗口设置
     */
    void saveWindowSettings();

    /**
     * @brief 更新UI状态
     */
    void updateUIState();

    /**
     * @brief 更新知识库统计显示
     */
    void updateKnowledgeBaseStats();

private:
    // 核心组件
    RAGEngine* m_ragEngine;
    ConfigManager* m_configManager;
    
    // UI组件
    std::unique_ptr<ChatWidget> m_chatWidget;
    std::unique_ptr<DocumentManagerWidget> m_documentManager;
    std::unique_ptr<SettingsDialog> m_settingsDialog;
    std::unique_ptr<VoiceController> m_voiceController;
    
    // 布局组件
    QWidget* m_centralWidget;
    QSplitter* m_mainSplitter;
    QVBoxLayout* m_mainLayout;
    
    // 菜单和工具栏
    QMenuBar* m_menuBar;
    QToolBar* m_toolBar;
    QStatusBar* m_statusBar;
    
    // 状态栏组件
    QLabel* m_statusLabel;
    QLabel* m_statsLabel;
    QProgressBar* m_progressBar;
    QPushButton* m_voiceInputButton;
    QPushButton* m_voiceOutputButton;
    
    // 动作
    QAction* m_newChatAction;
    QAction* m_openDocumentAction;
    QAction* m_saveChatAction;
    QAction* m_exportChatAction;
    QAction* m_settingsAction;
    QAction* m_aboutAction;
    QAction* m_exitAction;
    QAction* m_clearKnowledgeBaseAction;
    QAction* m_refreshStatsAction;
    
    // 状态变量
    bool m_initialized;
    bool m_voiceInputEnabled;
    bool m_voiceOutputEnabled;
};
