#pragma once

#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <vector>
#include <memory>

struct DocumentChunk;

/**
 * @brief 向量搜索结果
 */
struct VectorSearchResult
{
    QString chunkId;        ///< 文档块ID
    double similarity;      ///< 相似度分数
    QJsonObject metadata;   ///< 元数据
    
    VectorSearchResult() : similarity(0.0) {}
};

/**
 * @brief 向量存储配置
 */
struct VectorStoreConfig
{
    QString storePath;          ///< 存储路径
    QString collectionName;     ///< 集合名称
    int vectorDimension;        ///< 向量维度
    QString distanceMetric;     ///< 距离度量方式
    int maxElements;            ///< 最大元素数量
    
    VectorStoreConfig()
        : storePath("./data/vector_db")
        , collectionName("kylin_documents")
        , vectorDimension(768)
        , distanceMetric("cosine")
        , maxElements(1000000)
    {}
};

/**
 * @brief 向量存储抽象基类
 */
class VectorStore
{
public:
    /**
     * @brief 构造函数
     */
    VectorStore() = default;

    /**
     * @brief 析构函数
     */
    virtual ~VectorStore() = default;

    /**
     * @brief 初始化向量存储
     * @param config 配置参数
     * @return 初始化是否成功
     */
    virtual bool initialize(const VectorStoreConfig& config) = 0;

    /**
     * @brief 添加向量
     * @param chunkId 文档块ID
     * @param vector 向量数据
     * @param metadata 元数据
     * @return 添加是否成功
     */
    virtual bool addVector(const QString& chunkId, 
                          const std::vector<float>& vector,
                          const QJsonObject& metadata) = 0;

    /**
     * @brief 批量添加向量
     * @param chunks 文档块列表
     * @param vectors 向量列表
     * @return 添加是否成功
     */
    virtual bool addVectors(const std::vector<DocumentChunk>& chunks,
                           const std::vector<std::vector<float>>& vectors) = 0;

    /**
     * @brief 搜索相似向量
     * @param queryVector 查询向量
     * @param topK 返回数量
     * @param threshold 相似度阈值
     * @return 搜索结果
     */
    virtual std::vector<VectorSearchResult> search(const std::vector<float>& queryVector,
                                                  int topK = 5,
                                                  double threshold = 0.0) = 0;

    /**
     * @brief 删除向量
     * @param chunkId 文档块ID
     * @return 删除是否成功
     */
    virtual bool removeVector(const QString& chunkId) = 0;

    /**
     * @brief 批量删除向量
     * @param chunkIds 文档块ID列表
     * @return 删除是否成功
     */
    virtual bool removeVectors(const QStringList& chunkIds) = 0;

    /**
     * @brief 更新向量
     * @param chunkId 文档块ID
     * @param vector 新向量数据
     * @param metadata 新元数据
     * @return 更新是否成功
     */
    virtual bool updateVector(const QString& chunkId,
                             const std::vector<float>& vector,
                             const QJsonObject& metadata) = 0;

    /**
     * @brief 获取向量数量
     * @return 向量数量
     */
    virtual int getVectorCount() const = 0;

    /**
     * @brief 获取存储统计信息
     * @return 统计信息
     */
    virtual QJsonObject getStats() const = 0;

    /**
     * @brief 清空所有向量
     * @return 清空是否成功
     */
    virtual bool clear() = 0;

    /**
     * @brief 保存索引到磁盘
     * @return 保存是否成功
     */
    virtual bool save() = 0;

    /**
     * @brief 从磁盘加载索引
     * @return 加载是否成功
     */
    virtual bool load() = 0;

    /**
     * @brief 检查向量是否存在
     * @param chunkId 文档块ID
     * @return 是否存在
     */
    virtual bool hasVector(const QString& chunkId) const = 0;

    /**
     * @brief 获取向量
     * @param chunkId 文档块ID
     * @return 向量数据，如果不存在返回空向量
     */
    virtual std::vector<float> getVector(const QString& chunkId) const = 0;

protected:
    /**
     * @brief 计算余弦相似度
     * @param vec1 向量1
     * @param vec2 向量2
     * @return 相似度分数
     */
    static double calculateCosineSimilarity(const std::vector<float>& vec1,
                                          const std::vector<float>& vec2);

    /**
     * @brief 计算欧几里得距离
     * @param vec1 向量1
     * @param vec2 向量2
     * @return 距离值
     */
    static double calculateEuclideanDistance(const std::vector<float>& vec1,
                                           const std::vector<float>& vec2);

    /**
     * @brief 向量归一化
     * @param vector 输入向量
     * @return 归一化后的向量
     */
    static std::vector<float> normalizeVector(const std::vector<float>& vector);
};

/**
 * @brief 基于内存的简单向量存储实现
 */
class MemoryVectorStore : public VectorStore
{
public:
    MemoryVectorStore();
    ~MemoryVectorStore() override;

    bool initialize(const VectorStoreConfig& config) override;
    bool addVector(const QString& chunkId, 
                  const std::vector<float>& vector,
                  const QJsonObject& metadata) override;
    bool addVectors(const std::vector<DocumentChunk>& chunks,
                   const std::vector<std::vector<float>>& vectors) override;
    std::vector<VectorSearchResult> search(const std::vector<float>& queryVector,
                                          int topK = 5,
                                          double threshold = 0.0) override;
    bool removeVector(const QString& chunkId) override;
    bool removeVectors(const QStringList& chunkIds) override;
    bool updateVector(const QString& chunkId,
                     const std::vector<float>& vector,
                     const QJsonObject& metadata) override;
    int getVectorCount() const override;
    QJsonObject getStats() const override;
    bool clear() override;
    bool save() override;
    bool load() override;
    bool hasVector(const QString& chunkId) const override;
    std::vector<float> getVector(const QString& chunkId) const override;

private:
    struct VectorEntry
    {
        QString chunkId;
        std::vector<float> vector;
        QJsonObject metadata;
    };

    std::vector<VectorEntry> m_vectors;
    VectorStoreConfig m_config;
    bool m_initialized;
};
