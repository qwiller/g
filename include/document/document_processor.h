#pragma once

#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <vector>

/**
 * @brief 文档块结构
 */
struct DocumentChunk
{
    QString chunkId;        ///< 块ID
    QString content;        ///< 文本内容
    QJsonObject metadata;   ///< 元数据
    int tokenCount;         ///< Token数量
    
    DocumentChunk() : tokenCount(0) {}
};

/**
 * @brief 文档处理结果
 */
struct ProcessResult
{
    bool success;                           ///< 处理是否成功
    QString errorMessage;                   ///< 错误信息
    std::vector<DocumentChunk> chunks;      ///< 文档块列表
    QJsonObject documentMetadata;           ///< 文档元数据
    
    ProcessResult() : success(false) {}
};

class PDFParser;
class MarkdownParser;
class TextParser;
class ChunkManager;

/**
 * @brief 文档处理器主类
 * 
 * 负责协调各种文档格式的解析和处理
 */
class DocumentProcessor
{
public:
    /**
     * @brief 构造函数
     */
    DocumentProcessor();

    /**
     * @brief 析构函数
     */
    ~DocumentProcessor();

    /**
     * @brief 初始化文档处理器
     * @param chunkSize 分块大小
     * @param overlapSize 重叠大小
     * @return 初始化是否成功
     */
    bool initialize(int chunkSize = 500, int overlapSize = 100);

    /**
     * @brief 处理文档文件
     * @param filePath 文件路径
     * @return 处理结果
     */
    ProcessResult processDocument(const QString& filePath);

    /**
     * @brief 处理文本内容
     * @param content 文本内容
     * @param sourceInfo 来源信息
     * @return 处理结果
     */
    ProcessResult processText(const QString& content, const QJsonObject& sourceInfo = QJsonObject());

    /**
     * @brief 检查文件格式是否支持
     * @param filePath 文件路径
     * @return 是否支持
     */
    bool isSupportedFormat(const QString& filePath) const;

    /**
     * @brief 获取支持的文件格式列表
     * @return 支持的格式列表
     */
    QStringList getSupportedFormats() const;

    /**
     * @brief 设置分块参数
     * @param chunkSize 分块大小
     * @param overlapSize 重叠大小
     */
    void setChunkParameters(int chunkSize, int overlapSize);

    /**
     * @brief 清理临时文件
     */
    void cleanupTempFiles();

private:
    /**
     * @brief 检测文件编码
     * @param filePath 文件路径
     * @return 编码格式
     */
    QString detectEncoding(const QString& filePath) const;

    /**
     * @brief 清洗文本内容
     * @param text 原始文本
     * @return 清洗后的文本
     */
    QString cleanText(const QString& text) const;

    /**
     * @brief 提取文档元数据
     * @param filePath 文件路径
     * @param content 文档内容
     * @return 元数据对象
     */
    QJsonObject extractMetadata(const QString& filePath, const QString& content) const;

    /**
     * @brief 计算Token数量（简单估算）
     * @param text 文本内容
     * @return Token数量
     */
    int estimateTokenCount(const QString& text) const;

private:
    std::unique_ptr<PDFParser> m_pdfParser;
    std::unique_ptr<MarkdownParser> m_markdownParser;
    std::unique_ptr<TextParser> m_textParser;
    std::unique_ptr<ChunkManager> m_chunkManager;
    
    QStringList m_supportedFormats;
    int m_chunkSize;
    int m_overlapSize;
    QString m_tempDir;
    bool m_initialized;
};
