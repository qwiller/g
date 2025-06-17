#include "rag/rag_engine.h"
#include "rag/vector_store.h"
#include "rag/retriever.h"
#include "rag/generator.h"
#include "ai/kylin_ai_client.h"
#include "document/document_processor.h"

#include <QDebug>
#include <QDateTime>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QElapsedTimer>
#include <algorithm>

RAGEngine::RAGEngine()
    : m_vectorStore(std::make_unique<MemoryVectorStore>())
    , m_retriever(std::make_unique<Retriever>())
    , m_generator(std::make_unique<Generator>())
    , m_kylinAI(std::make_unique<KylinAIClient>())
    , m_initialized(false)
{
}

RAGEngine::~RAGEngine() = default;

bool RAGEngine::initialize(const RAGConfig& config)
{
    m_config = config;
    
    // 初始化向量存储
    VectorStoreConfig vectorConfig;
    vectorConfig.vectorDimension = 768; // 默认向量维度
    
    if (!m_vectorStore->initialize(vectorConfig)) {
        m_lastError = "向量存储初始化失败";
        return false;
    }
    
    // 初始化检索器
    RetrieverConfig retrieverConfig;
    retrieverConfig.topK = config.topK;
    retrieverConfig.similarityThreshold = config.similarityThreshold;
    retrieverConfig.useReranking = config.useReranking;
    
    if (!m_retriever->initialize(retrieverConfig, m_vectorStore.get())) {
        m_lastError = "检索器初始化失败";
        return false;
    }
    
    // 初始化生成器
    GeneratorConfig generatorConfig;
    generatorConfig.maxTokens = config.maxTokens;
    generatorConfig.temperature = config.temperature;
    
    if (!m_generator->initialize(generatorConfig)) {
        m_lastError = "生成器初始化失败";
        return false;
    }
    
    // 初始化麒麟AI客户端
    KylinAIConfig kylinConfig;
    kylinConfig.textEmbeddingEnabled = true;
    kylinConfig.embeddingDimension = 768;
    
    if (!m_kylinAI->initialize(kylinConfig)) {
        qWarning() << "麒麟AI客户端初始化失败，将使用备用方案";
    }
    
    // 设置默认提示词模板
    if (m_config.promptTemplate.isEmpty()) {
        m_config.promptTemplate = 
            "你是银河麒麟系统专家。基于以下文档片段回答用户问题：\n\n"
            "【文档上下文】\n{context}\n\n"
            "【用户问题】\n{question}\n\n"
            "【回答要求】\n"
            "1. 仅基于提供的文档内容回答\n"
            "2. 如信息不足，明确说明\"文档中未找到相关信息\"\n"
            "3. 涉及操作步骤时，提供具体可执行的命令\n"
            "4. 使用麒麟系统标准术语\n\n"
            "回答：";
    }
    
    m_initialized = true;
    qDebug() << "RAG引擎初始化完成";
    
    return true;
}

bool RAGEngine::addDocuments(const std::vector<DocumentChunk>& chunks)
{
    if (!m_initialized) {
        m_lastError = "RAG引擎未初始化";
        return false;
    }
    
    if (chunks.empty()) {
        m_lastError = "文档块列表为空";
        return false;
    }
    
    // 批量向量化文本
    QStringList texts;
    for (const auto& chunk : chunks) {
        texts.append(chunk.content);
    }
    
    std::vector<std::vector<float>> vectors;
    
    // 尝试使用麒麟AI进行向量化
    if (m_kylinAI->isAvailable()) {
        auto embeddingResults = m_kylinAI->embedTexts(texts);
        for (const auto& result : embeddingResults) {
            if (result.success) {
                vectors.push_back(result.vector);
            } else {
                qWarning() << "文本向量化失败:" << result.errorMessage;
                // 使用零向量作为备用
                vectors.push_back(std::vector<float>(768, 0.0f));
            }
        }
    } else {
        // 备用方案：生成随机向量（仅用于测试）
        qWarning() << "麒麟AI不可用，使用随机向量";
        for (size_t i = 0; i < chunks.size(); ++i) {
            std::vector<float> randomVector(768);
            for (float& val : randomVector) {
                val = static_cast<float>(rand()) / RAND_MAX - 0.5f;
            }
            vectors.push_back(randomVector);
        }
    }
    
    // 添加到向量存储
    if (!m_vectorStore->addVectors(chunks, vectors)) {
        m_lastError = "向量存储添加失败";
        return false;
    }
    
    qDebug() << "成功添加文档块数量:" << chunks.size();
    return true;
}

bool RAGEngine::removeDocument(const QString& documentId)
{
    if (!m_initialized) {
        m_lastError = "RAG引擎未初始化";
        return false;
    }
    
    // 查找所有属于该文档的块
    QStringList chunkIds;
    auto stats = m_vectorStore->getStats();
    // 这里需要根据实际的向量存储实现来查找相关块
    
    if (!chunkIds.isEmpty()) {
        return m_vectorStore->removeVectors(chunkIds);
    }
    
    return true;
}

RAGResult RAGEngine::query(const QString& question, const QJsonObject& context)
{
    RAGResult result;
    QElapsedTimer timer;
    timer.start();
    
    if (!m_initialized) {
        result.errorMessage = "RAG引擎未初始化";
        return result;
    }
    
    if (question.trimmed().isEmpty()) {
        result.errorMessage = "问题不能为空";
        return result;
    }
    
    try {
        // 1. 检索相关文档
        auto retrievedChunks = m_retriever->retrieve(question, m_config.topK);
        
        if (retrievedChunks.empty()) {
            result.errorMessage = "未找到相关文档";
            result.answer = "抱歉，我在知识库中没有找到与您问题相关的信息。请尝试重新表述您的问题或添加更多相关文档。";
            result.confidence = 0.0;
            result.processingTime = timer.elapsed();
            return result;
        }
        
        // 2. 重排序（如果启用）
        if (m_config.useReranking) {
            retrievedChunks = rerankResults(retrievedChunks, question);
        }
        
        // 3. 构建提示词
        QString prompt = buildPrompt(question, retrievedChunks, context);
        
        // 4. 生成回答
        auto generationResult = m_generator->generate(prompt, context);
        
        if (!generationResult.success) {
            result.errorMessage = "回答生成失败: " + generationResult.errorMessage;
            return result;
        }
        
        // 5. 后处理回答
        result.answer = postProcessAnswer(generationResult.text, retrievedChunks);
        result.sources = retrievedChunks;
        result.confidence = calculateConfidence(result.answer, retrievedChunks, question);
        result.processingTime = timer.elapsed();
        
        // 6. 构建元数据
        result.metadata["question"] = question;
        result.metadata["retrieved_chunks_count"] = static_cast<int>(retrievedChunks.size());
        result.metadata["generation_tokens"] = generationResult.tokenCount;
        result.metadata["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        
        result.success = true;
        
        qDebug() << "RAG查询完成，处理时间:" << result.processingTime << "ms";
        
    } catch (const std::exception& e) {
        result.errorMessage = QString("RAG查询异常: %1").arg(e.what());
        qCritical() << result.errorMessage;
    }
    
    return result;
}

void RAGEngine::queryAsync(const QString& question, const QJsonObject& context, 
                          std::function<void(const RAGResult&)> callback)
{
    // 在实际实现中，这里应该使用线程池或异步任务
    // 为了简化，这里直接调用同步版本
    auto result = query(question, context);
    callback(result);
}

QJsonObject RAGEngine::getKnowledgeBaseStats() const
{
    if (!m_initialized) {
        return QJsonObject();
    }
    
    auto stats = m_vectorStore->getStats();
    stats["initialized"] = m_initialized;
    stats["config"] = QJsonObject{
        {"top_k", m_config.topK},
        {"similarity_threshold", m_config.similarityThreshold},
        {"max_tokens", m_config.maxTokens},
        {"temperature", m_config.temperature},
        {"use_reranking", m_config.useReranking}
    };
    
    return stats;
}

std::vector<DocumentChunk> RAGEngine::searchDocuments(const QString& query, int topK)
{
    if (!m_initialized) {
        return {};
    }
    
    return m_retriever->retrieve(query, topK);
}

void RAGEngine::updateConfig(const RAGConfig& config)
{
    m_config = config;
    
    if (m_initialized) {
        // 更新子组件配置
        RetrieverConfig retrieverConfig;
        retrieverConfig.topK = config.topK;
        retrieverConfig.similarityThreshold = config.similarityThreshold;
        retrieverConfig.useReranking = config.useReranking;
        
        m_retriever->updateConfig(retrieverConfig);
        
        GeneratorConfig generatorConfig = m_generator->getConfig();
        generatorConfig.maxTokens = config.maxTokens;
        generatorConfig.temperature = config.temperature;
        
        m_generator->updateConfig(generatorConfig);
    }
}

RAGConfig RAGEngine::getConfig() const
{
    return m_config;
}

bool RAGEngine::clearKnowledgeBase()
{
    if (!m_initialized) {
        return false;
    }
    
    return m_vectorStore->clear();
}

bool RAGEngine::saveKnowledgeBase(const QString& filePath)
{
    if (!m_initialized) {
        return false;
    }
    
    return m_vectorStore->save();
}

bool RAGEngine::loadKnowledgeBase(const QString& filePath)
{
    if (!m_initialized) {
        return false;
    }

    return m_vectorStore->load();
}

QString RAGEngine::buildPrompt(const QString& question,
                              const std::vector<DocumentChunk>& retrievedChunks,
                              const QJsonObject& context) const
{
    QString contextText;
    for (size_t i = 0; i < retrievedChunks.size(); ++i) {
        contextText += QString("【文档片段%1】\n%2\n\n").arg(i + 1).arg(retrievedChunks[i].content);
    }

    QString prompt = m_config.promptTemplate;
    prompt.replace("{context}", contextText);
    prompt.replace("{question}", question);

    return prompt;
}

QString RAGEngine::postProcessAnswer(const QString& answer,
                                    const std::vector<DocumentChunk>& sources) const
{
    QString processed = answer.trimmed();

    // 移除可能的提示词残留
    processed.remove(QRegularExpression("^(回答：|答案：|Answer:)\\s*"));

    return processed;
}

double RAGEngine::calculateConfidence(const QString& answer,
                                     const std::vector<DocumentChunk>& sources,
                                     const QString& question) const
{
    // 简单的置信度计算
    if (sources.empty()) return 0.0;
    if (answer.contains("未找到相关信息") || answer.contains("不知道")) return 0.2;

    // 基于检索到的文档数量和相似度
    double confidence = std::min(1.0, sources.size() / 5.0);

    return confidence;
}

std::vector<DocumentChunk> RAGEngine::rerankResults(const std::vector<DocumentChunk>& chunks,
                                                   const QString& question) const
{
    // 简单的重排序实现
    auto reranked = chunks;

    // 根据内容长度和关键词匹配进行重排序
    std::sort(reranked.begin(), reranked.end(), [&question](const DocumentChunk& a, const DocumentChunk& b) {
        int scoreA = 0, scoreB = 0;

        // 关键词匹配得分
        QStringList keywords = question.split(' ', Qt::SkipEmptyParts);
        for (const QString& keyword : keywords) {
            if (a.content.contains(keyword, Qt::CaseInsensitive)) scoreA++;
            if (b.content.contains(keyword, Qt::CaseInsensitive)) scoreB++;
        }

        return scoreA > scoreB;
    });

    return reranked;
}
