#include "ui/main_window.h"
#include "ui/chat_widget.h"
#include "ui/document_manager_widget.h"
#include "ui/settings_dialog.h"
#include "ui/voice_controller.h"
#include "rag/rag_engine.h"
#include "core/config_manager.h"

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QSettings>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ragEngine(nullptr)
    , m_configManager(nullptr)
    , m_chatWidget(std::make_unique<ChatWidget>())
    , m_documentManager(std::make_unique<DocumentManagerWidget>())
    , m_settingsDialog(std::make_unique<SettingsDialog>())
    , m_voiceController(std::make_unique<VoiceController>())
    , m_centralWidget(nullptr)
    , m_mainSplitter(nullptr)
    , m_mainLayout(nullptr)
    , m_menuBar(nullptr)
    , m_toolBar(nullptr)
    , m_statusBar(nullptr)
    , m_statusLabel(nullptr)
    , m_statsLabel(nullptr)
    , m_progressBar(nullptr)
    , m_voiceInputButton(nullptr)
    , m_voiceOutputButton(nullptr)
    , m_initialized(false)
    , m_voiceInputEnabled(false)
    , m_voiceOutputEnabled(false)
{
    setWindowTitle("银河麒麟智能问答助手");
    setWindowIcon(QIcon(":/icons/app_icon.png"));
    
    // 设置最小窗口大小
    setMinimumSize(800, 600);
    resize(1200, 800);
    
    createActions();
    createMenuBar();
    createToolBar();
    createStatusBar();
    createCentralWidget();
    
    applyTheme();
    loadWindowSettings();
}

MainWindow::~MainWindow()
{
    saveWindowSettings();
}

bool MainWindow::initialize(RAGEngine* ragEngine, ConfigManager* configManager)
{
    if (!ragEngine || !configManager) {
        qCritical() << "RAG引擎或配置管理器为空";
        return false;
    }
    
    m_ragEngine = ragEngine;
    m_configManager = configManager;
    
    // 初始化子组件
    if (!m_chatWidget->initialize(ragEngine)) {
        qCritical() << "聊天组件初始化失败";
        return false;
    }
    
    if (!m_documentManager->initialize(ragEngine)) {
        qCritical() << "文档管理组件初始化失败";
        return false;
    }
    
    if (!m_voiceController->initialize()) {
        qWarning() << "语音控制器初始化失败";
        // 语音功能失败不影响主要功能
    }
    
    connectSignals();
    updateUIState();
    updateKnowledgeBaseStats();
    
    m_initialized = true;
    showStatusMessage("银河麒麟智能问答助手已就绪", 3000);
    
    qDebug() << "主窗口初始化完成";
    return true;
}

void MainWindow::showStatusMessage(const QString& message, int timeout)
{
    if (m_statusLabel) {
        m_statusLabel->setText(message);
        if (timeout > 0) {
            QTimer::singleShot(timeout, [this]() {
                m_statusLabel->setText("就绪");
            });
        }
    }
}

void MainWindow::setProgressValue(int value)
{
    if (m_progressBar) {
        m_progressBar->setValue(value);
    }
}

void MainWindow::setProgressVisible(bool visible)
{
    if (m_progressBar) {
        m_progressBar->setVisible(visible);
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // 保存设置
    saveWindowSettings();
    
    // 询问是否确认退出
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认退出", "确定要退出银河麒麟智能问答助手吗？",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    
    // 保存窗口大小
    if (m_configManager && m_initialized) {
        m_configManager->setInt("window.width", event->size().width());
        m_configManager->setInt("window.height", event->size().height());
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    // 处理全局快捷键
    if (event->modifiers() & Qt::ControlModifier) {
        switch (event->key()) {
        case Qt::Key_N:
            onNewChat();
            return;
        case Qt::Key_O:
            onOpenDocument();
            return;
        case Qt::Key_S:
            onSaveChat();
            return;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            // Ctrl+Enter 发送消息
            if (m_chatWidget) {
                m_chatWidget->focusInput();
            }
            return;
        }
    }
    
    // F1 语音输入
    if (event->key() == Qt::Key_F1) {
        onToggleVoiceInput();
        return;
    }
    
    QMainWindow::keyPressEvent(event);
}

void MainWindow::onNewChat()
{
    if (m_chatWidget) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "新建对话", "确定要清空当前对话记录吗？",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No
        );
        
        if (reply == QMessageBox::Yes) {
            m_chatWidget->clearChat();
            showStatusMessage("已创建新对话", 2000);
        }
    }
}

void MainWindow::onOpenDocument()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择文档文件",
        QString(),
        "支持的文档 (*.pdf *.md *.txt);;PDF文件 (*.pdf);;Markdown文件 (*.md);;文本文件 (*.txt);;所有文件 (*)"
    );
    
    if (!fileName.isEmpty() && m_documentManager) {
        m_documentManager->addDocument(fileName);
    }
}

void MainWindow::onSaveChat()
{
    if (!m_chatWidget) return;
    
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "保存对话记录",
        QString("chat_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        "文本文件 (*.txt);;Markdown文件 (*.md);;所有文件 (*)"
    );
    
    if (!fileName.isEmpty()) {
        // 实现保存对话记录的逻辑
        showStatusMessage("对话记录已保存", 2000);
    }
}

void MainWindow::onExportChat()
{
    if (!m_chatWidget) return;
    
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "导出对话记录",
        QString("chat_export_%1.html").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        "HTML文件 (*.html);;PDF文件 (*.pdf);;所有文件 (*)"
    );
    
    if (!fileName.isEmpty()) {
        // 实现导出对话记录的逻辑
        showStatusMessage("对话记录已导出", 2000);
    }
}

void MainWindow::onShowSettings()
{
    if (m_settingsDialog) {
        m_settingsDialog->exec();
    }
}

void MainWindow::onShowAbout()
{
    QMessageBox::about(this, "关于",
        "<h3>银河麒麟智能问答助手 v1.0.0</h3>"
        "<p>基于RAG技术的智能问答系统</p>"
        "<p>专为银河麒麟操作系统优化</p>"
        "<p><b>技术特性：</b></p>"
        "<ul>"
        "<li>文档智能解析与分块</li>"
        "<li>向量化检索与语义匹配</li>"
        "<li>大语言模型生成回答</li>"
        "<li>语音识别与合成</li>"
        "</ul>"
        "<p>Copyright © 2024 麒麟软件</p>"
    );
}

void MainWindow::onToggleVoiceInput()
{
    m_voiceInputEnabled = !m_voiceInputEnabled;
    
    if (m_voiceInputButton) {
        m_voiceInputButton->setChecked(m_voiceInputEnabled);
        m_voiceInputButton->setText(m_voiceInputEnabled ? "停止录音" : "语音输入");
    }
    
    if (m_voiceInputEnabled) {
        showStatusMessage("开始语音输入...", 0);
        if (m_voiceController) {
            m_voiceController->startRecording();
        }
    } else {
        showStatusMessage("语音输入已停止", 2000);
        if (m_voiceController) {
            m_voiceController->stopRecording();
        }
    }
}

void MainWindow::onToggleVoiceOutput()
{
    m_voiceOutputEnabled = !m_voiceOutputEnabled;
    
    if (m_voiceOutputButton) {
        m_voiceOutputButton->setChecked(m_voiceOutputEnabled);
        m_voiceOutputButton->setToolTip(m_voiceOutputEnabled ? "关闭语音播报" : "开启语音播报");
    }
    
    showStatusMessage(m_voiceOutputEnabled ? "语音播报已开启" : "语音播报已关闭", 2000);
}

void MainWindow::onClearKnowledgeBase()
{
    QMessageBox::StandardButton reply = QMessageBox::warning(
        this, "清空知识库", 
        "确定要清空所有文档数据吗？此操作不可撤销！",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes && m_ragEngine) {
        if (m_ragEngine->clearKnowledgeBase()) {
            showStatusMessage("知识库已清空", 2000);
            updateKnowledgeBaseStats();
        } else {
            QMessageBox::critical(this, "错误", "清空知识库失败");
        }
    }
}

void MainWindow::onRefreshStats()
{
    updateKnowledgeBaseStats();
    showStatusMessage("统计信息已刷新", 2000);
}

void MainWindow::onDocumentAdded(bool success, const QString& message)
{
    if (success) {
        showStatusMessage("文档添加成功: " + message, 3000);
        updateKnowledgeBaseStats();
    } else {
        QMessageBox::warning(this, "文档添加失败", message);
        showStatusMessage("文档添加失败", 3000);
    }
}

void MainWindow::onQueryCompleted(bool success, int processingTime)
{
    setProgressVisible(false);

    if (success) {
        showStatusMessage(QString("查询完成，用时 %1ms").arg(processingTime), 3000);
    } else {
        showStatusMessage("查询失败", 3000);
    }
}

void MainWindow::onSpeechRecognized(const QString& text)
{
    if (m_chatWidget && !text.isEmpty()) {
        m_chatWidget->setInputText(text);
        showStatusMessage("语音识别完成", 2000);
    }

    // 自动停止录音
    if (m_voiceInputEnabled) {
        onToggleVoiceInput();
    }
}

void MainWindow::onSpeechSynthesized()
{
    showStatusMessage("语音播报完成", 2000);
}

void MainWindow::createMenuBar()
{
    m_menuBar = menuBar();

    // 文件菜单
    QMenu* fileMenu = m_menuBar->addMenu("文件(&F)");
    fileMenu->addAction(m_newChatAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_openDocumentAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_saveChatAction);
    fileMenu->addAction(m_exportChatAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_exitAction);

    // 工具菜单
    QMenu* toolsMenu = m_menuBar->addMenu("工具(&T)");
    toolsMenu->addAction(m_clearKnowledgeBaseAction);
    toolsMenu->addAction(m_refreshStatsAction);
    toolsMenu->addSeparator();
    toolsMenu->addAction(m_settingsAction);

    // 帮助菜单
    QMenu* helpMenu = m_menuBar->addMenu("帮助(&H)");
    helpMenu->addAction(m_aboutAction);
}

void MainWindow::createToolBar()
{
    m_toolBar = addToolBar("主工具栏");
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    m_toolBar->addAction(m_newChatAction);
    m_toolBar->addAction(m_openDocumentAction);
    m_toolBar->addSeparator();

    // 语音控制按钮
    m_voiceInputButton = new QPushButton("语音输入");
    m_voiceInputButton->setCheckable(true);
    m_voiceInputButton->setIcon(QIcon(":/icons/microphone.png"));
    m_toolBar->addWidget(m_voiceInputButton);

    m_voiceOutputButton = new QPushButton("语音播报");
    m_voiceOutputButton->setCheckable(true);
    m_voiceOutputButton->setIcon(QIcon(":/icons/speaker.png"));
    m_toolBar->addWidget(m_voiceOutputButton);

    m_toolBar->addSeparator();
    m_toolBar->addAction(m_settingsAction);
}

void MainWindow::createStatusBar()
{
    m_statusBar = statusBar();

    // 状态标签
    m_statusLabel = new QLabel("就绪");
    m_statusBar->addWidget(m_statusLabel);

    // 进度条
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    m_progressBar->setMaximumWidth(200);
    m_statusBar->addPermanentWidget(m_progressBar);

    // 统计信息标签
    m_statsLabel = new QLabel("知识库: 0 文档");
    m_statusBar->addPermanentWidget(m_statsLabel);
}

void MainWindow::createCentralWidget()
{
    m_centralWidget = new QWidget();
    setCentralWidget(m_centralWidget);

    // 创建主分割器
    m_mainSplitter = new QSplitter(Qt::Horizontal);

    // 添加聊天组件
    m_mainSplitter->addWidget(m_chatWidget.get());

    // 添加文档管理组件
    m_mainSplitter->addWidget(m_documentManager.get());

    // 设置分割比例
    m_mainSplitter->setStretchFactor(0, 2); // 聊天区域占2/3
    m_mainSplitter->setStretchFactor(1, 1); // 文档管理区域占1/3

    // 创建主布局
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->addWidget(m_mainSplitter);
    m_mainLayout->setContentsMargins(5, 5, 5, 5);
}

void MainWindow::createActions()
{
    // 新建对话
    m_newChatAction = new QAction(QIcon(":/icons/new_chat.png"), "新建对话(&N)", this);
    m_newChatAction->setShortcut(QKeySequence::New);
    m_newChatAction->setStatusTip("创建新的对话");
    connect(m_newChatAction, &QAction::triggered, this, &MainWindow::onNewChat);

    // 打开文档
    m_openDocumentAction = new QAction(QIcon(":/icons/open_document.png"), "打开文档(&O)", this);
    m_openDocumentAction->setShortcut(QKeySequence::Open);
    m_openDocumentAction->setStatusTip("添加文档到知识库");
    connect(m_openDocumentAction, &QAction::triggered, this, &MainWindow::onOpenDocument);

    // 保存对话
    m_saveChatAction = new QAction(QIcon(":/icons/save.png"), "保存对话(&S)", this);
    m_saveChatAction->setShortcut(QKeySequence::Save);
    m_saveChatAction->setStatusTip("保存当前对话记录");
    connect(m_saveChatAction, &QAction::triggered, this, &MainWindow::onSaveChat);

    // 导出对话
    m_exportChatAction = new QAction(QIcon(":/icons/export.png"), "导出对话(&E)", this);
    m_exportChatAction->setStatusTip("导出对话记录为文件");
    connect(m_exportChatAction, &QAction::triggered, this, &MainWindow::onExportChat);

    // 设置
    m_settingsAction = new QAction(QIcon(":/icons/settings.png"), "设置(&S)", this);
    m_settingsAction->setStatusTip("打开设置对话框");
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::onShowSettings);

    // 关于
    m_aboutAction = new QAction("关于(&A)", this);
    m_aboutAction->setStatusTip("显示关于信息");
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onShowAbout);

    // 退出
    m_exitAction = new QAction("退出(&X)", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip("退出应用程序");
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);

    // 清空知识库
    m_clearKnowledgeBaseAction = new QAction(QIcon(":/icons/clear.png"), "清空知识库(&C)", this);
    m_clearKnowledgeBaseAction->setStatusTip("清空所有文档数据");
    connect(m_clearKnowledgeBaseAction, &QAction::triggered, this, &MainWindow::onClearKnowledgeBase);

    // 刷新统计
    m_refreshStatsAction = new QAction(QIcon(":/icons/refresh.png"), "刷新统计(&R)", this);
    m_refreshStatsAction->setStatusTip("刷新知识库统计信息");
    connect(m_refreshStatsAction, &QAction::triggered, this, &MainWindow::onRefreshStats);
}
