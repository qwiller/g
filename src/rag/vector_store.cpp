#include "rag/vector_store.h"
#include "document/document_processor.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <algorithm>
#include <cmath>

MemoryVectorStore::MemoryVectorStore()
    : m_initialized(false)
    , m_vectorDimension(768)
{
}

MemoryVectorStore::~MemoryVectorStore() = default;

bool MemoryVectorStore::initialize(const VectorStoreConfig& config)
{
    m_config = config;
    m_vectorDimension = config.vectorDimension;
    m_initialized = true;
    
    qDebug() << "内存向量存储初始化完成，向量维度:" << m_vectorDimension;
    return true;
}

bool MemoryVectorStore::addVectors(const std::vector<DocumentChunk>& chunks, 
                                  const std::vector<std::vector<float>>& vectors)
{
    if (!m_initialized) {
        qWarning() << "向量存储未初始化";
        return false;
    }
    
    if (chunks.size() != vectors.size()) {
        qWarning() << "文档块数量与向量数量不匹配";
        return false;
    }
    
    for (size_t i = 0; i < chunks.size(); ++i) {
        const auto& chunk = chunks[i];
        const auto& vector = vectors[i];
        
        if (vector.size() != static_cast<size_t>(m_vectorDimension)) {
            qWarning() << "向量维度不匹配，期望:" << m_vectorDimension << "实际:" << vector.size();
            continue;
        }
        
        VectorEntry entry;
        entry.id = chunk.chunkId;
        entry.vector = vector;
        entry.chunk = chunk;
        entry.timestamp = QDateTime::currentDateTime();
        
        m_vectors[chunk.chunkId] = entry;
    }
    
    qDebug() << "成功添加" << chunks.size() << "个向量，总数:" << m_vectors.size();
    return true;
}

bool MemoryVectorStore::removeVectors(const QStringList& ids)
{
    if (!m_initialized) {
        return false;
    }
    
    int removedCount = 0;
    for (const QString& id : ids) {
        if (m_vectors.erase(id) > 0) {
            removedCount++;
        }
    }
    
    qDebug() << "移除了" << removedCount << "个向量";
    return removedCount > 0;
}

std::vector<VectorSearchResult> MemoryVectorStore::search(const std::vector<float>& queryVector, 
                                                         int topK, 
                                                         double threshold)
{
    std::vector<VectorSearchResult> results;
    
    if (!m_initialized || queryVector.size() != static_cast<size_t>(m_vectorDimension)) {
        return results;
    }
    
    // 计算所有向量的相似度
    std::vector<std::pair<double, QString>> similarities;
    
    for (const auto& pair : m_vectors) {
        const VectorEntry& entry = pair.second;
        double similarity = calculateCosineSimilarity(queryVector, entry.vector);
        
        if (similarity >= threshold) {
            similarities.emplace_back(similarity, entry.id);
        }
    }
    
    // 按相似度排序
    std::sort(similarities.begin(), similarities.end(), 
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // 取前topK个结果
    int resultCount = std::min(topK, static_cast<int>(similarities.size()));
    results.reserve(resultCount);
    
    for (int i = 0; i < resultCount; ++i) {
        const auto& sim = similarities[i];
        const VectorEntry& entry = m_vectors[sim.second];
        
        VectorSearchResult result;
        result.chunk = entry.chunk;
        result.similarity = sim.first;
        result.distance = 1.0 - sim.first; // 余弦距离
        
        results.push_back(result);
    }
    
    qDebug() << "向量搜索完成，查询到" << results.size() << "个结果";
    return results;
}

bool MemoryVectorStore::updateVector(const QString& id, const std::vector<float>& vector)
{
    if (!m_initialized) {
        return false;
    }
    
    auto it = m_vectors.find(id);
    if (it == m_vectors.end()) {
        return false;
    }
    
    if (vector.size() != static_cast<size_t>(m_vectorDimension)) {
        return false;
    }
    
    it->second.vector = vector;
    it->second.timestamp = QDateTime::currentDateTime();
    
    return true;
}

QJsonObject MemoryVectorStore::getStats() const
{
    QJsonObject stats;
    
    stats["initialized"] = m_initialized;
    stats["vector_count"] = static_cast<int>(m_vectors.size());
    stats["vector_dimension"] = m_vectorDimension;
    stats["storage_type"] = "memory";
    
    if (!m_vectors.empty()) {
        // 计算存储统计
        size_t totalMemory = m_vectors.size() * (m_vectorDimension * sizeof(float) + 1024); // 估算
        stats["estimated_memory_mb"] = static_cast<int>(totalMemory / (1024 * 1024));
        
        // 最新和最旧的条目时间
        QDateTime newest, oldest;
        bool first = true;
        
        for (const auto& pair : m_vectors) {
            const QDateTime& timestamp = pair.second.timestamp;
            if (first) {
                newest = oldest = timestamp;
                first = false;
            } else {
                if (timestamp > newest) newest = timestamp;
                if (timestamp < oldest) oldest = timestamp;
            }
        }
        
        stats["newest_entry"] = newest.toString(Qt::ISODate);
        stats["oldest_entry"] = oldest.toString(Qt::ISODate);
    }
    
    return stats;
}

bool MemoryVectorStore::clear()
{
    if (!m_initialized) {
        return false;
    }
    
    m_vectors.clear();
    qDebug() << "向量存储已清空";
    return true;
}

bool MemoryVectorStore::save()
{
    if (!m_initialized) {
        return false;
    }
    
    // 创建保存目录
    QDir().mkpath("data/vector_db");
    
    QJsonObject root;
    QJsonArray vectorArray;
    
    for (const auto& pair : m_vectors) {
        const VectorEntry& entry = pair.second;
        
        QJsonObject vectorObj;
        vectorObj["id"] = entry.id;
        vectorObj["timestamp"] = entry.timestamp.toString(Qt::ISODate);
        
        // 保存向量数据
        QJsonArray vectorData;
        for (float value : entry.vector) {
            vectorData.append(static_cast<double>(value));
        }
        vectorObj["vector"] = vectorData;
        
        // 保存文档块数据
        QJsonObject chunkObj;
        chunkObj["chunk_id"] = entry.chunk.chunkId;
        chunkObj["content"] = entry.chunk.content;
        chunkObj["chunk_index"] = entry.chunk.chunkIndex;
        chunkObj["token_count"] = entry.chunk.tokenCount;
        chunkObj["metadata"] = entry.chunk.metadata;
        
        vectorObj["chunk"] = chunkObj;
        vectorArray.append(vectorObj);
    }
    
    root["vectors"] = vectorArray;
    root["config"] = QJsonObject{
        {"vector_dimension", m_vectorDimension},
        {"version", "1.0"},
        {"saved_time", QDateTime::currentDateTime().toString(Qt::ISODate)}
    };
    
    QFile file("data/vector_db/vectors.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "无法保存向量数据库:" << file.errorString();
        return false;
    }
    
    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();
    
    qDebug() << "向量数据库已保存，包含" << m_vectors.size() << "个向量";
    return true;
}

bool MemoryVectorStore::load()
{
    QFile file("data/vector_db/vectors.json");
    if (!file.exists()) {
        qDebug() << "向量数据库文件不存在，从空数据库开始";
        return true;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法读取向量数据库:" << file.errorString();
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "向量数据库JSON解析错误:" << error.errorString();
        return false;
    }
    
    QJsonObject root = doc.object();
    QJsonArray vectorArray = root["vectors"].toArray();
    
    m_vectors.clear();
    
    for (const QJsonValue& value : vectorArray) {
        QJsonObject vectorObj = value.toObject();
        
        VectorEntry entry;
        entry.id = vectorObj["id"].toString();
        entry.timestamp = QDateTime::fromString(vectorObj["timestamp"].toString(), Qt::ISODate);
        
        // 加载向量数据
        QJsonArray vectorData = vectorObj["vector"].toArray();
        entry.vector.reserve(vectorData.size());
        for (const QJsonValue& val : vectorData) {
            entry.vector.push_back(static_cast<float>(val.toDouble()));
        }
        
        // 加载文档块数据
        QJsonObject chunkObj = vectorObj["chunk"].toObject();
        entry.chunk.chunkId = chunkObj["chunk_id"].toString();
        entry.chunk.content = chunkObj["content"].toString();
        entry.chunk.chunkIndex = chunkObj["chunk_index"].toInt();
        entry.chunk.tokenCount = chunkObj["token_count"].toInt();
        entry.chunk.metadata = chunkObj["metadata"].toObject();
        
        m_vectors[entry.id] = entry;
    }
    
    qDebug() << "向量数据库加载完成，包含" << m_vectors.size() << "个向量";
    return true;
}

double MemoryVectorStore::calculateCosineSimilarity(const std::vector<float>& a, 
                                                   const std::vector<float>& b) const
{
    if (a.size() != b.size()) {
        return 0.0;
    }
    
    double dotProduct = 0.0;
    double normA = 0.0;
    double normB = 0.0;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA == 0.0 || normB == 0.0) {
        return 0.0;
    }
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}
