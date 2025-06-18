#include "document/chunk_manager.h"

#include <QUuid>
#include <QDebug>
#include <QRegularExpression>
#include <algorithm>

ChunkManager::ChunkManager()
    : m_chunkSize(500)
    , m_chunkOverlap(100)
{
}

ChunkManager::~ChunkManager() = default;

void ChunkManager::setChunkParameters(int chunkSize, int chunkOverlap)
{
    m_chunkSize = chunkSize;
    m_chunkOverlap = chunkOverlap;
    
    qDebug() << "分块参数设置 - 大小:" << m_chunkSize << "重叠:" << m_chunkOverlap;
}

std::vector<DocumentChunk> ChunkManager::createChunks(const QString& text, const QJsonObject& metadata)
{
    std::vector<DocumentChunk> chunks;
    
    if (text.isEmpty()) {
        qWarning() << "输入文本为空，无法创建分块";
        return chunks;
    }
    
    // 预处理文本
    QString cleanedText = preprocessText(text);
    
    // 根据文本长度选择分块策略
    if (cleanedText.length() <= m_chunkSize) {
        // 文本较短，创建单个块
        chunks.push_back(createSingleChunk(cleanedText, metadata, 0));
    } else {
        // 文本较长，进行分块
        chunks = performChunking(cleanedText, metadata);
    }
    
    qDebug() << "文本分块完成，共创建" << chunks.size() << "个块";
    
    return chunks;
}

std::vector<DocumentChunk> ChunkManager::mergeChunks(const std::vector<DocumentChunk>& chunks)
{
    if (chunks.size() <= 1) {
        return chunks;
    }
    
    std::vector<DocumentChunk> merged;
    QString currentContent;
    QJsonObject currentMetadata;
    int currentTokens = 0;
    
    for (const auto& chunk : chunks) {
        if (currentTokens + chunk.tokenCount <= m_chunkSize) {
            // 可以合并
            if (!currentContent.isEmpty()) {
                currentContent += "\n\n";
            }
            currentContent += chunk.content;
            currentTokens += chunk.tokenCount;
            
            // 合并元数据
            if (currentMetadata.isEmpty()) {
                currentMetadata = chunk.metadata;
            }
        } else {
            // 创建合并后的块
            if (!currentContent.isEmpty()) {
                DocumentChunk mergedChunk = createSingleChunk(currentContent, currentMetadata, merged.size());
                merged.push_back(mergedChunk);
            }
            
            // 开始新的合并块
            currentContent = chunk.content;
            currentMetadata = chunk.metadata;
            currentTokens = chunk.tokenCount;
        }
    }
    
    // 添加最后一个块
    if (!currentContent.isEmpty()) {
        DocumentChunk mergedChunk = createSingleChunk(currentContent, currentMetadata, merged.size());
        merged.push_back(mergedChunk);
    }
    
    qDebug() << "块合并完成，从" << chunks.size() << "个块合并为" << merged.size() << "个块";
    
    return merged;
}

int ChunkManager::estimateTokenCount(const QString& text) const
{
    // 简单的Token估算
    // 中文：1个字符约等于1个token
    // 英文：1个单词约等于1个token
    
    int chineseChars = 0;
    int englishWords = 0;
    
    for (const QChar& ch : text) {
        if (ch.unicode() >= 0x4E00 && ch.unicode() <= 0x9FFF) {
            chineseChars++;
        }
    }
    
    // 计算英文单词数
    QStringList words = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    for (const QString& word : words) {
        bool hasEnglish = false;
        for (const QChar& ch : word) {
            if (ch.isLetter() && ch.unicode() < 128) {
                hasEnglish = true;
                break;
            }
        }
        if (hasEnglish) {
            englishWords++;
        }
    }
    
    // 估算总token数
    int totalTokens = chineseChars + englishWords;
    
    // 添加标点符号和其他字符的估算
    int otherChars = text.length() - chineseChars;
    totalTokens += otherChars / 4; // 假设4个其他字符约等于1个token
    
    return std::max(1, totalTokens);
}

QString ChunkManager::preprocessText(const QString& text) const
{
    QString processed = text;
    
    // 规范化空白字符
    processed.replace(QRegularExpression("\\s+"), " ");
    
    // 规范化换行符
    processed.replace(QRegularExpression("\\n\\s*\\n"), "\n\n");
    
    // 移除首尾空白
    processed = processed.trimmed();
    
    return processed;
}

std::vector<DocumentChunk> ChunkManager::performChunking(const QString& text, const QJsonObject& metadata)
{
    std::vector<DocumentChunk> chunks;
    
    // 首先尝试按段落分块
    QStringList paragraphs = text.split(QRegularExpression("\\n\\s*\\n"), Qt::SkipEmptyParts);
    
    if (paragraphs.size() > 1) {
        chunks = chunkByParagraphs(paragraphs, metadata);
    } else {
        // 如果没有明显的段落，按句子分块
        chunks = chunkBySentences(text, metadata);
    }
    
    return chunks;
}

std::vector<DocumentChunk> ChunkManager::chunkByParagraphs(const QStringList& paragraphs, const QJsonObject& metadata)
{
    std::vector<DocumentChunk> chunks;
    QString currentChunk;
    int currentTokens = 0;
    
    for (const QString& paragraph : paragraphs) {
        int paragraphTokens = estimateTokenCount(paragraph);
        
        if (currentTokens + paragraphTokens <= m_chunkSize) {
            // 添加到当前块
            if (!currentChunk.isEmpty()) {
                currentChunk += "\n\n";
            }
            currentChunk += paragraph;
            currentTokens += paragraphTokens;
        } else {
            // 创建当前块
            if (!currentChunk.isEmpty()) {
                DocumentChunk chunk = createSingleChunk(currentChunk, metadata, chunks.size());
                chunks.push_back(chunk);
            }
            
            // 检查单个段落是否过长
            if (paragraphTokens > m_chunkSize) {
                // 段落过长，需要进一步分割
                auto subChunks = chunkBySentences(paragraph, metadata);
                for (auto& subChunk : subChunks) {
                    subChunk.chunkIndex = chunks.size();
                    chunks.push_back(subChunk);
                }
                currentChunk.clear();
                currentTokens = 0;
            } else {
                // 开始新块
                currentChunk = paragraph;
                currentTokens = paragraphTokens;
            }
        }
    }
    
    // 添加最后一个块
    if (!currentChunk.isEmpty()) {
        DocumentChunk chunk = createSingleChunk(currentChunk, metadata, chunks.size());
        chunks.push_back(chunk);
    }
    
    return chunks;
}

std::vector<DocumentChunk> ChunkManager::chunkBySentences(const QString& text, const QJsonObject& metadata)
{
    std::vector<DocumentChunk> chunks;
    
    // 简单的句子分割
    QStringList sentences = text.split(QRegularExpression("[。！？.!?]\\s*"), Qt::SkipEmptyParts);
    
    QString currentChunk;
    int currentTokens = 0;
    
    for (const QString& sentence : sentences) {
        QString trimmedSentence = sentence.trimmed();
        if (trimmedSentence.isEmpty()) continue;
        
        int sentenceTokens = estimateTokenCount(trimmedSentence);
        
        if (currentTokens + sentenceTokens <= m_chunkSize) {
            // 添加到当前块
            if (!currentChunk.isEmpty()) {
                currentChunk += "。";
            }
            currentChunk += trimmedSentence;
            currentTokens += sentenceTokens;
        } else {
            // 创建当前块
            if (!currentChunk.isEmpty()) {
                DocumentChunk chunk = createSingleChunk(currentChunk, metadata, chunks.size());
                chunks.push_back(chunk);
            }
            
            // 开始新块
            currentChunk = trimmedSentence;
            currentTokens = sentenceTokens;
        }
    }
    
    // 添加最后一个块
    if (!currentChunk.isEmpty()) {
        DocumentChunk chunk = createSingleChunk(currentChunk, metadata, chunks.size());
        chunks.push_back(chunk);
    }
    
    return chunks;
}

DocumentChunk ChunkManager::createSingleChunk(const QString& content, const QJsonObject& metadata, int index)
{
    DocumentChunk chunk;
    
    chunk.chunkId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    chunk.content = content;
    chunk.chunkIndex = index;
    chunk.tokenCount = estimateTokenCount(content);
    chunk.metadata = metadata;
    
    // 添加分块相关的元数据
    chunk.metadata["chunk_size"] = m_chunkSize;
    chunk.metadata["chunk_overlap"] = m_chunkOverlap;
    chunk.metadata["chunk_method"] = "ChunkManager";
    chunk.metadata["created_time"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    return chunk;
}
