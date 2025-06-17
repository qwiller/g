#pragma once

#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <vector>

struct DocumentChunk;
class VectorStore;
class Retriever;
class Generator;
class KylinAIClient;

/**
 * @brief RAG查询结果
 */
struct RAGResult
{
    bool success;                           ///< 查询是否成功
    QString errorMessage;                   ///< 错误信息
    QString answer;                         ///< 生成的答案
    std::vector<DocumentChunk> sources;     ///< 相关文档片段
    QJsonObject metadata;                   ///< 查询元数据
    double confidence;                      ///< 置信度
    int processingTime;                     ///< 处理时间（毫秒）
    
    RAGResult() : success(false), confidence(0.0), processingTime(0) {}
};

/**
 * @brief RAG引擎配置
 */
struct RAGConfig
{
    int topK;                   ///< 检索的文档数量
    double similarityThreshold; ///< 相似度阈值
    int maxTokens;              ///< 最大生成token数
    double temperature;         ///< 生成温度
    bool useReranking;          ///< 是否使用重排序
    QString promptTemplate;     ///< 提示词模板
    
    RAGConfig()
        : topK(5)
        , similarityThreshold(0.7)
        , maxTokens(2048)
        , temperature(0.7)
        , useReranking(true)
        , promptTemplate("")
    {}
};

/**
 * @brief RAG核心引擎
 * 
 * 整合向量检索和生成式AI，提供完整的RAG问答功能
 */
class RAGEngine
{
public:
    /**
     * @brief 构造函数
     */
    RAGEngine();

    /**
     * @brief 析构函数
     */
    ~RAGEngine();

    /**
     * @brief 初始化RAG引擎
     * @param config RAG配置
     * @return 初始化是否成功
     */
    bool initialize(const RAGConfig& config);

    /**
     * @brief 添加文档到知识库
     * @param chunks 文档块列表
     * @return 添加是否成功
     */
    bool addDocuments(const std::vector<DocumentChunk>& chunks);

    /**
     * @brief 删除文档
     * @param documentId 文档ID
     * @return 删除是否成功
     */
    bool removeDocument(const QString& documentId);

    /**
     * @brief 执行RAG查询
     * @param question 用户问题
     * @param context 额外上下文
     * @return 查询结果
     */
    RAGResult query(const QString& question, const QJsonObject& context = QJsonObject());

    /**
     * @brief 异步执行RAG查询
     * @param question 用户问题
     * @param context 额外上下文
     * @param callback 回调函数
     */
    void queryAsync(const QString& question, const QJsonObject& context, 
                   std::function<void(const RAGResult&)> callback);

    /**
     * @brief 获取知识库统计信息
     * @return 统计信息
     */
    QJsonObject getKnowledgeBaseStats() const;

    /**
     * @brief 搜索相关文档
     * @param query 查询文本
     * @param topK 返回数量
     * @return 相关文档列表
     */
    std::vector<DocumentChunk> searchDocuments(const QString& query, int topK = 5);

    /**
     * @brief 更新RAG配置
     * @param config 新配置
     */
    void updateConfig(const RAGConfig& config);

    /**
     * @brief 获取当前配置
     * @return 当前配置
     */
    RAGConfig getConfig() const;

    /**
     * @brief 清空知识库
     * @return 清空是否成功
     */
    bool clearKnowledgeBase();

    /**
     * @brief 保存知识库到文件
     * @param filePath 文件路径
     * @return 保存是否成功
     */
    bool saveKnowledgeBase(const QString& filePath);

    /**
     * @brief 从文件加载知识库
     * @param filePath 文件路径
     * @return 加载是否成功
     */
    bool loadKnowledgeBase(const QString& filePath);

private:
    /**
     * @brief 构建提示词
     * @param question 用户问题
     * @param retrievedChunks 检索到的文档片段
     * @param context 额外上下文
     * @return 构建的提示词
     */
    QString buildPrompt(const QString& question, 
                       const std::vector<DocumentChunk>& retrievedChunks,
                       const QJsonObject& context) const;

    /**
     * @brief 后处理生成的答案
     * @param answer 原始答案
     * @param sources 来源文档
     * @return 处理后的答案
     */
    QString postProcessAnswer(const QString& answer, 
                             const std::vector<DocumentChunk>& sources) const;

    /**
     * @brief 计算答案置信度
     * @param answer 生成的答案
     * @param sources 来源文档
     * @param question 原始问题
     * @return 置信度分数
     */
    double calculateConfidence(const QString& answer,
                              const std::vector<DocumentChunk>& sources,
                              const QString& question) const;

    /**
     * @brief 重排序检索结果
     * @param chunks 原始检索结果
     * @param question 用户问题
     * @return 重排序后的结果
     */
    std::vector<DocumentChunk> rerankResults(const std::vector<DocumentChunk>& chunks,
                                           const QString& question) const;

private:
    std::unique_ptr<VectorStore> m_vectorStore;
    std::unique_ptr<Retriever> m_retriever;
    std::unique_ptr<Generator> m_generator;
    std::unique_ptr<KylinAIClient> m_kylinAI;
    
    RAGConfig m_config;
    bool m_initialized;
    mutable QString m_lastError;
};
