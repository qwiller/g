#pragma once

#include <QString>
#include <QJsonObject>
#include <vector>
#include <memory>

struct DocumentChunk;
class VectorStore;

/**
 * @brief 检索器配置
 */
struct RetrieverConfig
{
    int topK;                   ///< 检索数量
    double similarityThreshold; ///< 相似度阈值
    bool useReranking;          ///< 是否使用重排序
    
    RetrieverConfig()
        : topK(5)
        , similarityThreshold(0.7)
        , useReranking(true)
    {}
};

/**
 * @brief 文档检索器
 */
class Retriever
{
public:
    Retriever();
    ~Retriever();
    
    bool initialize(const RetrieverConfig& config, VectorStore* vectorStore);
    std::vector<DocumentChunk> retrieve(const QString& query, int topK = 5);
    void updateConfig(const RetrieverConfig& config);
    
private:
    RetrieverConfig m_config;
    VectorStore* m_vectorStore;
    bool m_initialized;
};
