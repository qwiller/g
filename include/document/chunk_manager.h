#pragma once

#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <vector>

struct DocumentChunk; // 前向声明

/**
 * @brief 分块策略枚举
 */
enum class ChunkStrategy
{
    FixedSize,      ///< 固定大小分块
    Semantic,       ///< 语义分块
    Paragraph,      ///< 段落分块
    Sentence        ///< 句子分块
};

/**
 * @brief 分块参数配置
 */
struct ChunkConfig
{
    int chunkSize;              ///< 分块大小（token数）
    int overlapSize;            ///< 重叠大小（token数）
    ChunkStrategy strategy;     ///< 分块策略
    bool preserveStructure;     ///< 是否保持结构
    int minChunkSize;           ///< 最小分块大小
    int maxChunkSize;           ///< 最大分块大小
    
    ChunkConfig() 
        : chunkSize(500)
        , overlapSize(100)
        , strategy(ChunkStrategy::Semantic)
        , preserveStructure(true)
        , minChunkSize(100)
        , maxChunkSize(1000)
    {}
};

/**
 * @brief 文档分块管理器
 * 
 * 负责将长文档智能分割成适合向量化的文本块
 */
class ChunkManager
{
public:
    /**
     * @brief 构造函数
     */
    ChunkManager();

    /**
     * @brief 析构函数
     */
    ~ChunkManager();

    /**
     * @brief 设置分块配置
     * @param config 分块配置
     */
    void setConfig(const ChunkConfig& config);

    /**
     * @brief 获取当前分块配置
     * @return 分块配置
     */
    ChunkConfig getConfig() const;

    /**
     * @brief 对文档进行分块
     * @param content 文档内容
     * @param metadata 文档元数据
     * @return 分块结果列表
     */
    std::vector<DocumentChunk> chunkDocument(const QString& content, const QJsonObject& metadata);

    /**
     * @brief 合并相邻的小块
     * @param chunks 原始分块列表
     * @return 合并后的分块列表
     */
    std::vector<DocumentChunk> mergeSmallChunks(const std::vector<DocumentChunk>& chunks);

    /**
     * @brief 验证分块质量
     * @param chunks 分块列表
     * @return 质量评分（0-1）
     */
    double validateChunkQuality(const std::vector<DocumentChunk>& chunks);

private:
    /**
     * @brief 固定大小分块
     * @param content 文档内容
     * @param metadata 元数据
     * @return 分块列表
     */
    std::vector<DocumentChunk> fixedSizeChunking(const QString& content, const QJsonObject& metadata);

    /**
     * @brief 语义分块
     * @param content 文档内容
     * @param metadata 元数据
     * @return 分块列表
     */
    std::vector<DocumentChunk> semanticChunking(const QString& content, const QJsonObject& metadata);

    /**
     * @brief 段落分块
     * @param content 文档内容
     * @param metadata 元数据
     * @return 分块列表
     */
    std::vector<DocumentChunk> paragraphChunking(const QString& content, const QJsonObject& metadata);

    /**
     * @brief 句子分块
     * @param content 文档内容
     * @param metadata 元数据
     * @return 分块列表
     */
    std::vector<DocumentChunk> sentenceChunking(const QString& content, const QJsonObject& metadata);

    /**
     * @brief 检测语义边界
     * @param text 文本内容
     * @return 边界位置列表
     */
    QList<int> detectSemanticBoundaries(const QString& text);

    /**
     * @brief 分割段落
     * @param text 文本内容
     * @return 段落列表
     */
    QStringList splitParagraphs(const QString& text);

    /**
     * @brief 分割句子
     * @param text 文本内容
     * @return 句子列表
     */
    QStringList splitSentences(const QString& text);

    /**
     * @brief 估算文本token数量
     * @param text 文本内容
     * @return token数量
     */
    int estimateTokenCount(const QString& text);

    /**
     * @brief 创建文档块
     * @param content 块内容
     * @param index 块索引
     * @param metadata 原始元数据
     * @return 文档块对象
     */
    DocumentChunk createChunk(const QString& content, int index, const QJsonObject& metadata);

    /**
     * @brief 添加重叠内容
     * @param chunks 分块列表
     * @return 添加重叠后的分块列表
     */
    std::vector<DocumentChunk> addOverlap(const std::vector<DocumentChunk>& chunks);

    /**
     * @brief 计算文本相似度
     * @param text1 文本1
     * @param text2 文本2
     * @return 相似度（0-1）
     */
    double calculateTextSimilarity(const QString& text1, const QString& text2);

private:
    ChunkConfig m_config;
    int m_chunkCounter;
};
