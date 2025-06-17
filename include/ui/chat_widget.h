#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QLabel>
#include <QFrame>
#include <QTimer>
#include <QPropertyAnimation>
#include <memory>

struct RAGResult;
class RAGEngine;

/**
 * @brief 消息类型枚举
 */
enum class MessageType
{
    User,       ///< 用户消息
    Assistant,  ///< 助手回复
    System      ///< 系统消息
};

/**
 * @brief 消息项结构
 */
struct ChatMessage
{
    MessageType type;       ///< 消息类型
    QString content;        ///< 消息内容
    QString timestamp;      ///< 时间戳
    QStringList sources;    ///< 来源文档（仅助手消息）
    double confidence;      ///< 置信度（仅助手消息）
    
    ChatMessage() : type(MessageType::System), confidence(0.0) {}
};

/**
 * @brief 消息气泡组件
 */
class MessageBubble : public QFrame
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param message 消息内容
     * @param parent 父组件
     */
    explicit MessageBubble(const ChatMessage& message, QWidget* parent = nullptr);

    /**
     * @brief 设置消息内容
     * @param message 消息内容
     */
    void setMessage(const ChatMessage& message);

    /**
     * @brief 获取消息内容
     * @return 消息内容
     */
    ChatMessage getMessage() const;

    /**
     * @brief 开始打字动画
     */
    void startTypingAnimation();

    /**
     * @brief 停止打字动画
     */
    void stopTypingAnimation();

protected:
    /**
     * @brief 绘制事件
     * @param event 绘制事件
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief 鼠标双击事件
     * @param event 鼠标事件
     */
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private slots:
    /**
     * @brief 显示来源文档
     */
    void onShowSources();

    /**
     * @brief 复制消息内容
     */
    void onCopyMessage();

private:
    /**
     * @brief 设置样式
     */
    void setupStyle();

    /**
     * @brief 创建布局
     */
    void createLayout();

    /**
     * @brief 格式化消息内容
     * @param content 原始内容
     * @return 格式化后的内容
     */
    QString formatContent(const QString& content);

private:
    ChatMessage m_message;
    QVBoxLayout* m_layout;
    QLabel* m_contentLabel;
    QLabel* m_timestampLabel;
    QLabel* m_confidenceLabel;
    QPushButton* m_sourcesButton;
    QTimer* m_typingTimer;
    QPropertyAnimation* m_fadeAnimation;
};

/**
 * @brief 聊天组件
 * 
 * 提供聊天界面，包括消息显示、输入和发送功能
 */
class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父组件
     */
    explicit ChatWidget(QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~ChatWidget();

    /**
     * @brief 初始化聊天组件
     * @param ragEngine RAG引擎实例
     * @return 初始化是否成功
     */
    bool initialize(RAGEngine* ragEngine);

    /**
     * @brief 添加消息
     * @param message 消息内容
     */
    void addMessage(const ChatMessage& message);

    /**
     * @brief 清空聊天记录
     */
    void clearChat();

    /**
     * @brief 设置输入文本
     * @param text 输入文本
     */
    void setInputText(const QString& text);

    /**
     * @brief 获取输入文本
     * @return 输入文本
     */
    QString getInputText() const;

    /**
     * @brief 获取聊天历史
     * @return 消息列表
     */
    QList<ChatMessage> getChatHistory() const;

    /**
     * @brief 设置输入框焦点
     */
    void focusInput();

    /**
     * @brief 启用/禁用输入
     * @param enabled 是否启用
     */
    void setInputEnabled(bool enabled);

signals:
    /**
     * @brief 发送消息信号
     * @param message 消息内容
     */
    void messageSent(const QString& message);

    /**
     * @brief 查询完成信号
     * @param success 是否成功
     * @param processingTime 处理时间
     */
    void queryCompleted(bool success, int processingTime);

    /**
     * @brief 请求语音输入信号
     */
    void voiceInputRequested();

    /**
     * @brief 请求语音播报信号
     * @param text 要播报的文本
     */
    void voiceOutputRequested(const QString& text);

private slots:
    /**
     * @brief 发送消息
     */
    void onSendMessage();

    /**
     * @brief 处理输入框回车键
     */
    void onInputReturnPressed();

    /**
     * @brief 处理RAG查询结果
     * @param result 查询结果
     */
    void onRAGQueryFinished(const RAGResult& result);

    /**
     * @brief 语音输入按钮点击
     */
    void onVoiceInputClicked();

    /**
     * @brief 语音播报按钮点击
     */
    void onVoiceOutputClicked();

    /**
     * @brief 输入文本改变
     */
    void onInputTextChanged();

private:
    /**
     * @brief 创建UI布局
     */
    void createLayout();

    /**
     * @brief 连接信号和槽
     */
    void connectSignals();

    /**
     * @brief 应用样式
     */
    void applyStyle();

    /**
     * @brief 滚动到底部
     */
    void scrollToBottom();

    /**
     * @brief 显示正在思考动画
     */
    void showThinkingAnimation();

    /**
     * @brief 隐藏正在思考动画
     */
    void hideThinkingAnimation();

    /**
     * @brief 验证输入内容
     * @param text 输入文本
     * @return 是否有效
     */
    bool validateInput(const QString& text);

    /**
     * @brief 处理快捷键
     * @param key 按键
     * @param modifiers 修饰键
     * @return 是否处理
     */
    bool handleShortcut(int key, Qt::KeyboardModifiers modifiers);

protected:
    /**
     * @brief 键盘按键事件
     * @param event 按键事件
     */
    void keyPressEvent(QKeyEvent* event) override;

private:
    // 核心组件
    RAGEngine* m_ragEngine;
    
    // UI组件
    QVBoxLayout* m_mainLayout;
    QScrollArea* m_chatScrollArea;
    QWidget* m_chatContainer;
    QVBoxLayout* m_chatLayout;
    
    // 输入区域
    QHBoxLayout* m_inputLayout;
    QLineEdit* m_inputEdit;
    QPushButton* m_sendButton;
    QPushButton* m_voiceInputButton;
    QPushButton* m_voiceOutputButton;
    
    // 状态组件
    QLabel* m_thinkingLabel;
    QTimer* m_thinkingTimer;
    
    // 数据
    QList<ChatMessage> m_chatHistory;
    bool m_initialized;
    bool m_isProcessing;
};
