#include "document/document_processor.h"
#include "document/pdf_parser.h"
#include "document/markdown_parser.h"
#include "document/text_parser.h"
#include "document/chunk_manager.h"

#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QDateTime>
#include <QCryptographicHash>
#include <QTextBoundaryFinder>

DocumentProcessor::DocumentProcessor()
    : m_pdfParser(std::make_unique<PDFParser>())
    , m_markdownParser(std::make_unique<MarkdownParser>())
    , m_textParser(std::make_unique<TextParser>())
    , m_chunkManager(std::make_unique<ChunkManager>())
    , m_chunkSize(500)
    , m_overlapSize(100)
    , m_initialized(false)
{
    m_supportedFormats << ".pdf" << ".md" << ".markdown" << ".txt" << ".text";
}

DocumentProcessor::~DocumentProcessor() = default;

bool DocumentProcessor::initialize(int chunkSize, int overlapSize)
{
    m_chunkSize = chunkSize;
    m_overlapSize = overlapSize;
    
    // 设置分块配置
    ChunkConfig config;
    config.chunkSize = chunkSize;
    config.overlapSize = overlapSize;
    config.strategy = ChunkStrategy::Semantic;
    config.preserveStructure = true;
    
    m_chunkManager->setConfig(config);
    
    // 创建临时目录
    m_tempDir = QDir::tempPath() + "/kylin_qa_temp";
    QDir().mkpath(m_tempDir);
    
    m_initialized = true;
    qDebug() << "文档处理器初始化完成，分块大小:" << chunkSize << "重叠大小:" << overlapSize;
    
    return true;
}

ProcessResult DocumentProcessor::processDocument(const QString& filePath)
{
    ProcessResult result;
    
    if (!m_initialized) {
        result.errorMessage = "文档处理器未初始化";
        return result;
    }
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        result.errorMessage = "文件不存在: " + filePath;
        return result;
    }
    
    if (!isSupportedFormat(filePath)) {
        result.errorMessage = "不支持的文件格式: " + fileInfo.suffix();
        return result;
    }
    
    QString content;
    QJsonObject metadata;
    
    // 根据文件类型选择解析器
    QString suffix = fileInfo.suffix().toLower();
    
    if (suffix == "pdf") {
        auto pdfResult = m_pdfParser->parsePDF(filePath);
        if (!pdfResult.success) {
            result.errorMessage = "PDF解析失败: " + pdfResult.errorMessage;
            return result;
        }
        content = pdfResult.content;
        metadata = pdfResult.metadata;
    }
    else if (suffix == "md" || suffix == "markdown") {
        auto mdResult = m_markdownParser->parseMarkdown(filePath);
        if (!mdResult.success) {
            result.errorMessage = "Markdown解析失败: " + mdResult.errorMessage;
            return result;
        }
        content = mdResult.content;
        metadata = mdResult.metadata;
    }
    else if (suffix == "txt" || suffix == "text") {
        auto textResult = m_textParser->parseTextFile(filePath);
        if (!textResult.success) {
            result.errorMessage = "文本解析失败: " + textResult.errorMessage;
            return result;
        }
        content = textResult.content;
        metadata = textResult.metadata;
    }
    
    // 清洗文本
    content = cleanText(content);
    
    // 添加文件信息到元数据
    metadata["file_path"] = filePath;
    metadata["file_name"] = fileInfo.fileName();
    metadata["file_size"] = fileInfo.size();
    metadata["processed_time"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    // 进行文档分块
    result.chunks = m_chunkManager->chunkDocument(content, metadata);
    result.documentMetadata = metadata;
    result.success = true;
    
    qDebug() << "文档处理完成:" << filePath << "生成块数:" << result.chunks.size();
    
    return result;
}

ProcessResult DocumentProcessor::processText(const QString& content, const QJsonObject& sourceInfo)
{
    ProcessResult result;
    
    if (!m_initialized) {
        result.errorMessage = "文档处理器未初始化";
        return result;
    }
    
    if (content.isEmpty()) {
        result.errorMessage = "文本内容为空";
        return result;
    }
    
    // 清洗文本
    QString cleanedContent = cleanText(content);
    
    // 生成元数据
    QJsonObject metadata = sourceInfo;
    metadata["content_length"] = cleanedContent.length();
    metadata["processed_time"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    metadata["content_hash"] = QString(QCryptographicHash::hash(cleanedContent.toUtf8(), QCryptographicHash::Md5).toHex());
    
    // 进行文档分块
    result.chunks = m_chunkManager->chunkDocument(cleanedContent, metadata);
    result.documentMetadata = metadata;
    result.success = true;
    
    return result;
}

bool DocumentProcessor::isSupportedFormat(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);
    QString suffix = "." + fileInfo.suffix().toLower();
    return m_supportedFormats.contains(suffix);
}

QStringList DocumentProcessor::getSupportedFormats() const
{
    return m_supportedFormats;
}

void DocumentProcessor::setChunkParameters(int chunkSize, int overlapSize)
{
    m_chunkSize = chunkSize;
    m_overlapSize = overlapSize;
    
    ChunkConfig config = m_chunkManager->getConfig();
    config.chunkSize = chunkSize;
    config.overlapSize = overlapSize;
    m_chunkManager->setConfig(config);
}

void DocumentProcessor::cleanupTempFiles()
{
    QDir tempDir(m_tempDir);
    if (tempDir.exists()) {
        tempDir.removeRecursively();
        qDebug() << "清理临时文件目录:" << m_tempDir;
    }
}

QString DocumentProcessor::detectEncoding(const QString& filePath) const
{
    return m_textParser->detectEncoding(filePath);
}

QString DocumentProcessor::cleanText(const QString& text) const
{
    QString cleaned = text;
    
    // 标准化换行符
    cleaned.replace("\r\n", "\n");
    cleaned.replace("\r", "\n");
    
    // 移除多余的空白字符
    cleaned = cleaned.simplified();
    
    // 移除控制字符（保留换行符和制表符）
    QString result;
    for (const QChar& ch : cleaned) {
        if (ch.isPrint() || ch == '\n' || ch == '\t') {
            result.append(ch);
        }
    }
    
    return result;
}

QJsonObject DocumentProcessor::extractMetadata(const QString& filePath, const QString& content) const
{
    QJsonObject metadata;
    QFileInfo fileInfo(filePath);
    
    metadata["file_path"] = filePath;
    metadata["file_name"] = fileInfo.fileName();
    metadata["file_size"] = fileInfo.size();
    metadata["file_extension"] = fileInfo.suffix();
    metadata["created_time"] = fileInfo.birthTime().toString(Qt::ISODate);
    metadata["modified_time"] = fileInfo.lastModified().toString(Qt::ISODate);
    metadata["content_length"] = content.length();
    metadata["estimated_tokens"] = estimateTokenCount(content);
    
    return metadata;
}

int DocumentProcessor::estimateTokenCount(const QString& text) const
{
    // 简单的中文token估算：中文字符按1个token计算，英文单词按平均4个字符1个token计算
    int chineseChars = 0;
    int englishChars = 0;
    
    for (const QChar& ch : text) {
        if (ch.script() == QChar::Script_Han) {
            chineseChars++;
        } else if (ch.isLetter()) {
            englishChars++;
        }
    }
    
    return chineseChars + (englishChars / 4);
}
