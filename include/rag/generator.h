#pragma once

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <memory>
#include <functional>

struct DocumentChunk;

/**
 * @brief 生成结果
 */
struct GenerationResult
{
    bool success;           ///< 生成是否成功
    QString errorMessage;   ///< 错误信息
    QString text;           ///< 生成的文本
    int tokenCount;         ///< 使用的token数量
    double confidence;      ///< 置信度
    int processingTime;     ///< 处理时间（毫秒）
    QJsonObject metadata;   ///< 额外元数据
    
    GenerationResult() : success(false), tokenCount(0), confidence(0.0), processingTime(0) {}
};

/**
 * @brief 生成器配置
 */
struct GeneratorConfig
{
    QString apiEndpoint;        ///< API端点
    QString apiKey;             ///< API密钥
    QString model;              ///< 模型名称
    int maxTokens;              ///< 最大token数
    double temperature;         ///< 生成温度
    double topP;                ///< Top-p采样
    int timeout;                ///< 超时时间（秒）
    int maxRetries;             ///< 最大重试次数
    
    GeneratorConfig()
        : apiEndpoint("https://api.siliconflow.cn/v1/chat/completions")
        , apiKey("")
        , model("deepseek-ai/DeepSeek-V3")
        , maxTokens(2048)
        , temperature(0.7)
        , topP(0.9)
        , timeout(30)
        , maxRetries(3)
    {}
};

/**
 * @brief 文本生成器
 * 
 * 负责调用大语言模型生成回答
 */
class Generator : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit Generator(QObject* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~Generator();

    /**
     * @brief 初始化生成器
     * @param config 配置参数
     * @return 初始化是否成功
     */
    bool initialize(const GeneratorConfig& config);

    /**
     * @brief 生成回答
     * @param prompt 提示词
     * @param context 上下文信息
     * @return 生成结果
     */
    GenerationResult generate(const QString& prompt, const QJsonObject& context = QJsonObject());

    /**
     * @brief 异步生成回答
     * @param prompt 提示词
     * @param context 上下文信息
     * @param callback 回调函数
     */
    void generateAsync(const QString& prompt, 
                      const QJsonObject& context,
                      std::function<void(const GenerationResult&)> callback);

    /**
     * @brief 基于检索结果生成回答
     * @param question 用户问题
     * @param retrievedChunks 检索到的文档片段
     * @param promptTemplate 提示词模板
     * @return 生成结果
     */
    GenerationResult generateWithRetrieval(const QString& question,
                                          const std::vector<DocumentChunk>& retrievedChunks,
                                          const QString& promptTemplate = "");

    /**
     * @brief 流式生成回答
     * @param prompt 提示词
     * @param callback 流式回调函数
     * @return 是否成功开始生成
     */
    bool generateStream(const QString& prompt,
                       std::function<void(const QString&, bool)> callback);

    /**
     * @brief 检查API连接
     * @return 连接是否正常
     */
    bool checkConnection();

    /**
     * @brief 获取模型信息
     * @return 模型信息
     */
    QJsonObject getModelInfo();

    /**
     * @brief 更新配置
     * @param config 新配置
     */
    void updateConfig(const GeneratorConfig& config);

    /**
     * @brief 获取当前配置
     * @return 当前配置
     */
    GeneratorConfig getConfig() const;

    /**
     * @brief 取消当前生成任务
     */
    void cancelGeneration();

signals:
    /**
     * @brief 生成完成信号
     * @param result 生成结果
     */
    void generationFinished(const GenerationResult& result);

    /**
     * @brief 流式生成数据信号
     * @param chunk 文本片段
     * @param isComplete 是否完成
     */
    void streamDataReceived(const QString& chunk, bool isComplete);

    /**
     * @brief 错误信号
     * @param errorMessage 错误信息
     */
    void errorOccurred(const QString& errorMessage);

private slots:
    /**
     * @brief 处理网络请求完成
     */
    void onNetworkRequestFinished();

    /**
     * @brief 处理网络错误
     * @param error 网络错误
     */
    void onNetworkError(QNetworkReply::NetworkError error);

private:
    /**
     * @brief 构建请求数据
     * @param prompt 提示词
     * @param context 上下文
     * @return 请求JSON对象
     */
    QJsonObject buildRequestData(const QString& prompt, const QJsonObject& context);

    /**
     * @brief 解析响应数据
     * @param responseData 响应JSON数据
     * @return 生成结果
     */
    GenerationResult parseResponse(const QJsonObject& responseData);

    /**
     * @brief 构建RAG提示词
     * @param question 用户问题
     * @param retrievedChunks 检索到的文档片段
     * @param promptTemplate 提示词模板
     * @return 构建的提示词
     */
    QString buildRAGPrompt(const QString& question,
                          const std::vector<DocumentChunk>& retrievedChunks,
                          const QString& promptTemplate);

    /**
     * @brief 处理API错误
     * @param errorCode HTTP错误码
     * @param errorMessage 错误信息
     * @return 处理后的错误信息
     */
    QString handleAPIError(int errorCode, const QString& errorMessage);

    /**
     * @brief 重试请求
     * @param requestData 请求数据
     * @param retryCount 当前重试次数
     * @return 生成结果
     */
    GenerationResult retryRequest(const QJsonObject& requestData, int retryCount);

    /**
     * @brief 验证生成结果
     * @param result 生成结果
     * @param question 原始问题
     * @return 是否有效
     */
    bool validateResult(const GenerationResult& result, const QString& question);

private:
    GeneratorConfig m_config;
    std::unique_ptr<QNetworkAccessManager> m_networkManager;
    QNetworkReply* m_currentReply;
    bool m_initialized;
    
    // 异步回调
    std::function<void(const GenerationResult&)> m_asyncCallback;
    std::function<void(const QString&, bool)> m_streamCallback;
    
    mutable QString m_lastError;
    
    // 默认提示词模板
    static const QString DEFAULT_RAG_PROMPT_TEMPLATE;
};
