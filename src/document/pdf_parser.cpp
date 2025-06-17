#include "document/pdf_parser.h"

#include <QFileInfo>
#include <QDebug>
#include <QDateTime>
#include <QTextCodec>
#include <QRegularExpression>
#include <poppler-document.h>
#include <poppler-page.h>

PDFParser::PDFParser() = default;

PDFParser::~PDFParser() = default;

PDFParseResult PDFParser::parsePDF(const QString& filePath)
{
    PDFParseResult result;
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        result.errorMessage = "PDF文件不存在: " + filePath;
        return result;
    }
    
    // 使用Poppler加载PDF文档
    std::unique_ptr<poppler::document> document(
        poppler::document::load_from_file(filePath.toStdString())
    );
    
    if (!document) {
        result.errorMessage = "无法打开PDF文件，可能文件损坏或需要密码";
        return result;
    }
    
    if (document->is_locked()) {
        result.errorMessage = "PDF文件被密码保护";
        return result;
    }
    
    result.pageCount = document->pages();
    if (result.pageCount == 0) {
        result.errorMessage = "PDF文件没有页面";
        return result;
    }
    
    QString fullText;
    
    // 逐页提取文本
    for (int i = 0; i < result.pageCount; ++i) {
        std::unique_ptr<poppler::page> page(document->create_page(i));
        if (!page) {
            qWarning() << "无法创建PDF页面:" << i;
            continue;
        }
        
        QString pageText = extractPageText(page.get(), i + 1);
        if (!pageText.isEmpty()) {
            fullText += pageText + "\n\n";
        }
    }
    
    // 清理和转换文本
    result.content = cleanPDFText(fullText);
    result.content = convertToUTF8(result.content);
    
    // 提取元数据
    result.metadata = extractPDFMetadata(document.get(), filePath);
    result.success = true;
    
    qDebug() << "PDF解析完成:" << filePath << "页数:" << result.pageCount << "文本长度:" << result.content.length();
    
    return result;
}

PDFParseResult PDFParser::parsePDFPages(const QString& filePath, int startPage, int endPage)
{
    PDFParseResult result;
    
    std::unique_ptr<poppler::document> document(
        poppler::document::load_from_file(filePath.toStdString())
    );
    
    if (!document || document->is_locked()) {
        result.errorMessage = "无法打开PDF文件";
        return result;
    }
    
    int totalPages = document->pages();
    if (endPage == -1) {
        endPage = totalPages;
    }
    
    if (startPage < 1 || startPage > totalPages || endPage < startPage || endPage > totalPages) {
        result.errorMessage = QString("页面范围无效: %1-%2，总页数: %3").arg(startPage).arg(endPage).arg(totalPages);
        return result;
    }
    
    QString fullText;
    
    for (int i = startPage - 1; i < endPage; ++i) {
        std::unique_ptr<poppler::page> page(document->create_page(i));
        if (page) {
            QString pageText = extractPageText(page.get(), i + 1);
            if (!pageText.isEmpty()) {
                fullText += pageText + "\n\n";
            }
        }
    }
    
    result.content = cleanPDFText(fullText);
    result.content = convertToUTF8(result.content);
    result.metadata = extractPDFMetadata(document.get(), filePath);
    result.pageCount = endPage - startPage + 1;
    result.success = true;
    
    return result;
}

bool PDFParser::isValidPDF(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || fileInfo.suffix().toLower() != "pdf") {
        return false;
    }
    
    std::unique_ptr<poppler::document> document(
        poppler::document::load_from_file(filePath.toStdString())
    );
    
    return document != nullptr;
}

QJsonObject PDFParser::getPDFInfo(const QString& filePath) const
{
    QJsonObject info;
    
    std::unique_ptr<poppler::document> document(
        poppler::document::load_from_file(filePath.toStdString())
    );
    
    if (!document) {
        return info;
    }
    
    info["page_count"] = document->pages();
    info["is_locked"] = document->is_locked();
    info["pdf_version"] = QString::fromStdString(document->pdf_version());
    
    // 获取文档信息
    auto metadata = document->metadata();
    for (const auto& pair : metadata) {
        QString key = QString::fromStdString(pair.first);
        QString value = QString::fromStdString(pair.second);
        info[key.toLower()] = value;
    }
    
    return info;
}

QString PDFParser::extractPageText(poppler::page* page, int pageNumber) const
{
    if (!page) {
        return QString();
    }
    
    // 提取页面文本
    std::string pageText = page->text().to_latin1();
    QString text = QString::fromStdString(pageText);
    
    // 如果文本为空，尝试UTF-8编码
    if (text.isEmpty()) {
        pageText = page->text().to_utf8();
        text = QString::fromUtf8(pageText.c_str());
    }
    
    return text;
}

QString PDFParser::cleanPDFText(const QString& text) const
{
    QString cleaned = text;
    
    // 移除页眉页脚模式（简单启发式）
    QStringList lines = cleaned.split('\n');
    QStringList cleanedLines;
    
    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        
        // 跳过可能的页眉页脚
        if (trimmed.length() < 3 || 
            trimmed.contains(QRegularExpression("^\\d+$")) ||  // 纯数字（页码）
            trimmed.contains(QRegularExpression("^第\\s*\\d+\\s*页")) ||  // 中文页码
            trimmed.contains(QRegularExpression("Page\\s+\\d+", QRegularExpression::CaseInsensitiveOption))) {
            continue;
        }
        
        cleanedLines.append(trimmed);
    }
    
    cleaned = cleanedLines.join('\n');
    
    // 修复断行问题
    cleaned.replace(QRegularExpression("([\\u4e00-\\u9fff])\\n([\\u4e00-\\u9fff])"), "\\1\\2");
    
    // 标准化空白字符
    cleaned.replace(QRegularExpression("\\s+"), " ");
    cleaned.replace(QRegularExpression("\\n\\s*\\n"), "\n\n");
    
    return cleaned.trimmed();
}

QJsonObject PDFParser::extractPDFMetadata(poppler::document* document, const QString& filePath) const
{
    QJsonObject metadata;
    QFileInfo fileInfo(filePath);
    
    // 基本文件信息
    metadata["file_path"] = filePath;
    metadata["file_name"] = fileInfo.fileName();
    metadata["file_size"] = fileInfo.size();
    metadata["page_count"] = document->pages();
    metadata["pdf_version"] = QString::fromStdString(document->pdf_version());
    metadata["is_encrypted"] = document->is_locked();
    
    // PDF文档属性
    auto docMetadata = document->metadata();
    for (const auto& pair : docMetadata) {
        QString key = QString::fromStdString(pair.first).toLower();
        QString value = QString::fromStdString(pair.second);
        
        if (!value.isEmpty()) {
            metadata[key] = value;
        }
    }
    
    // 添加处理时间
    metadata["processed_time"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    return metadata;
}

QString PDFParser::convertToUTF8(const QString& rawText) const
{
    // 如果文本已经是有效的UTF-8，直接返回
    if (rawText.toUtf8() == rawText.toLatin1()) {
        return rawText;
    }
    
    // 尝试不同的编码转换
    QStringList encodings = {"UTF-8", "GBK", "GB2312", "Big5", "ISO-8859-1"};
    
    for (const QString& encoding : encodings) {
        QTextCodec* codec = QTextCodec::codecForName(encoding.toLatin1());
        if (codec) {
            QString converted = codec->toUnicode(rawText.toLatin1());
            if (!converted.contains(QChar::ReplacementCharacter)) {
                return converted;
            }
        }
    }
    
    return rawText; // 如果所有转换都失败，返回原文本
}
