#pragma once

#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <memory>
#include <functional>

/**
 * @brief 语音识别结果
 */
struct SpeechRecognitionResult
{
    bool success;           ///< 识别是否成功
    QString errorMessage;   ///< 错误信息
    QString text;           ///< 识别的文本
    double confidence;      ///< 置信度
    int duration;           ///< 音频时长（毫秒）
    
    SpeechRecognitionResult() : success(false), confidence(0.0), duration(0) {}
};

/**
 * @brief 语音合成结果
 */
struct TextToSpeechResult
{
    bool success;           ///< 合成是否成功
    QString errorMessage;   ///< 错误信息
    QByteArray audioData;   ///< 音频数据
    QString audioFormat;    ///< 音频格式
    int duration;           ///< 音频时长（毫秒）
    
    TextToSpeechResult() : success(false), duration(0) {}
};

/**
 * @brief 文本向量化结果
 */
struct TextEmbeddingResult
{
    bool success;               ///< 向量化是否成功
    QString errorMessage;       ///< 错误信息
    std::vector<float> vector;  ///< 向量数据
    int dimension;              ///< 向量维度
    
    TextEmbeddingResult() : success(false), dimension(0) {}
};

/**
 * @brief 麒麟AI客户端配置
 */
struct KylinAIConfig
{
    // 语音识别配置
    bool speechRecognitionEnabled;
    QString speechLanguage;
    int sampleRate;
    
    // 语音合成配置
    bool textToSpeechEnabled;
    QString voiceName;
    double speechSpeed;
    double speechPitch;
    
    // 文本向量化配置
    bool textEmbeddingEnabled;
    QString embeddingModel;
    int embeddingDimension;
    
    // 网络配置
    int timeout;
    int maxRetries;
    
    KylinAIConfig()
        : speechRecognitionEnabled(true)
        , speechLanguage("zh-CN")
        , sampleRate(16000)
        , textToSpeechEnabled(true)
        , voiceName("zh-CN-XiaoxiaoNeural")
        , speechSpeed(1.0)
        , speechPitch(1.0)
        , textEmbeddingEnabled(true)
        , embeddingModel("text-embedding-ada-002")
        , embeddingDimension(768)
        , timeout(30000)
        , maxRetries(3)
    {}
};

/**
 * @brief 麒麟AI SDK客户端
 * 
 * 封装麒麟AI SDK的语音识别、语音合成和文本向量化功能
 */
class KylinAIClient : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit KylinAIClient(QObject* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~KylinAIClient();

    /**
     * @brief 初始化客户端
     * @param config 配置参数
     * @return 初始化是否成功
     */
    bool initialize(const KylinAIConfig& config);

    /**
     * @brief 检查麒麟AI SDK是否可用
     * @return 是否可用
     */
    bool isAvailable() const;

    /**
     * @brief 语音识别
     * @param audioData 音频数据
     * @param audioFormat 音频格式
     * @return 识别结果
     */
    SpeechRecognitionResult recognizeSpeech(const QByteArray& audioData, 
                                           const QString& audioFormat = "wav");

    /**
     * @brief 异步语音识别
     * @param audioData 音频数据
     * @param audioFormat 音频格式
     * @param callback 回调函数
     */
    void recognizeSpeechAsync(const QByteArray& audioData,
                             const QString& audioFormat,
                             std::function<void(const SpeechRecognitionResult&)> callback);

    /**
     * @brief 语音合成
     * @param text 要合成的文本
     * @param voiceName 语音名称
     * @return 合成结果
     */
    TextToSpeechResult synthesizeSpeech(const QString& text, 
                                       const QString& voiceName = "");

    /**
     * @brief 异步语音合成
     * @param text 要合成的文本
     * @param voiceName 语音名称
     * @param callback 回调函数
     */
    void synthesizeSpeechAsync(const QString& text,
                              const QString& voiceName,
                              std::function<void(const TextToSpeechResult&)> callback);

    /**
     * @brief 文本向量化
     * @param text 输入文本
     * @return 向量化结果
     */
    TextEmbeddingResult embedText(const QString& text);

    /**
     * @brief 批量文本向量化
     * @param texts 文本列表
     * @return 向量化结果列表
     */
    std::vector<TextEmbeddingResult> embedTexts(const QStringList& texts);

    /**
     * @brief 异步文本向量化
     * @param text 输入文本
     * @param callback 回调函数
     */
    void embedTextAsync(const QString& text,
                       std::function<void(const TextEmbeddingResult&)> callback);

    /**
     * @brief 获取支持的语音列表
     * @return 语音列表
     */
    QStringList getSupportedVoices() const;

    /**
     * @brief 获取支持的语言列表
     * @return 语言列表
     */
    QStringList getSupportedLanguages() const;

    /**
     * @brief 更新配置
     * @param config 新配置
     */
    void updateConfig(const KylinAIConfig& config);

    /**
     * @brief 获取当前配置
     * @return 当前配置
     */
    KylinAIConfig getConfig() const;

signals:
    /**
     * @brief 语音识别完成信号
     * @param result 识别结果
     */
    void speechRecognitionFinished(const SpeechRecognitionResult& result);

    /**
     * @brief 语音合成完成信号
     * @param result 合成结果
     */
    void textToSpeechFinished(const TextToSpeechResult& result);

    /**
     * @brief 文本向量化完成信号
     * @param result 向量化结果
     */
    void textEmbeddingFinished(const TextEmbeddingResult& result);

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

private:
    /**
     * @brief 检查麒麟AI SDK库
     * @return 是否可用
     */
    bool checkKylinAILibrary();

    /**
     * @brief 初始化语音识别
     * @return 初始化是否成功
     */
    bool initializeSpeechRecognition();

    /**
     * @brief 初始化语音合成
     * @return 初始化是否成功
     */
    bool initializeTextToSpeech();

    /**
     * @brief 初始化文本向量化
     * @return 初始化是否成功
     */
    bool initializeTextEmbedding();

    /**
     * @brief 发送HTTP请求（备用方案）
     * @param url 请求URL
     * @param data 请求数据
     * @return 响应数据
     */
    QJsonObject sendHttpRequest(const QString& url, const QJsonObject& data);

    /**
     * @brief 处理语音识别错误
     * @param errorCode 错误代码
     * @return 错误信息
     */
    QString handleSpeechRecognitionError(int errorCode);

    /**
     * @brief 处理语音合成错误
     * @param errorCode 错误代码
     * @return 错误信息
     */
    QString handleTextToSpeechError(int errorCode);

private:
    KylinAIConfig m_config;
    bool m_initialized;
    bool m_kylinAIAvailable;
    
    // 网络管理器（备用方案）
    std::unique_ptr<QNetworkAccessManager> m_networkManager;
    
    // 麒麟AI SDK句柄（如果可用）
    void* m_speechRecognitionHandle;
    void* m_textToSpeechHandle;
    void* m_textEmbeddingHandle;
    
    mutable QString m_lastError;
};
