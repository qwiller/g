#include "document/text_parser.h"

#include <QFileInfo>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>

TextParser::TextParser() = default;

TextParser::~TextParser() = default;

TextParseResult TextParser::parseText(const QString& filePath)
{
    TextParseResult result;
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        result.errorMessage = "文本文件不存在: " + filePath;
        return result;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        result.errorMessage = "无法打开文本文件: " + filePath;
        return result;
    }
    
    // 检测编码
    QByteArray rawData = file.readAll();
    file.close();
    
    QString encoding = detectEncoding(rawData);
    QString content = decodeText(rawData, encoding);
    
    // 清理文本
    result.content = cleanText(content);
    result.metadata = extractTextMetadata(content, filePath, encoding);
    result.success = true;
    
    qDebug() << "文本解析完成:" << filePath << "编码:" << encoding << "内容长度:" << result.content.length();
    
    return result;
}

bool TextParser::isValidText(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);
    QStringList validExtensions = {"txt", "text", "log", "csv", "tsv"};
    return fileInfo.exists() && validExtensions.contains(fileInfo.suffix().toLower());
}

QString TextParser::detectEncoding(const QByteArray& data) const
{
    // 检查BOM
    if (data.startsWith("\xEF\xBB\xBF")) {
        return "UTF-8";
    }
    if (data.startsWith("\xFF\xFE")) {
        return "UTF-16LE";
    }
    if (data.startsWith("\xFE\xFF")) {
        return "UTF-16BE";
    }
    
    // 尝试UTF-8解码
    QString utf8Text = QString::fromUtf8(data);
    if (!utf8Text.contains(QChar::ReplacementCharacter)) {
        return "UTF-8";
    }
    
    // 检查中文编码
    QStringList chineseEncodings = {"GBK", "GB2312", "Big5"};
    for (const QString& encoding : chineseEncodings) {
        QTextCodec* codec = QTextCodec::codecForName(encoding.toLocal8Bit());
        if (codec) {
            QString decoded = codec->toUnicode(data);
            if (!decoded.contains(QChar::ReplacementCharacter)) {
                return encoding;
            }
        }
    }
    
    // 默认使用UTF-8
    return "UTF-8";
}

QString TextParser::decodeText(const QByteArray& data, const QString& encoding) const
{
    if (encoding == "UTF-8") {
        return QString::fromUtf8(data);
    }
    
    QTextCodec* codec = QTextCodec::codecForName(encoding.toLocal8Bit());
    if (codec) {
        return codec->toUnicode(data);
    }
    
    // 备用方案
    return QString::fromLocal8Bit(data);
}

QString TextParser::cleanText(const QString& text) const
{
    QString cleaned = text;
    
    // 规范化换行符
    cleaned.replace("\r\n", "\n");
    cleaned.replace("\r", "\n");
    
    // 移除控制字符（保留换行符和制表符）
    cleaned.remove(QRegularExpression("[\\x00-\\x08\\x0B\\x0C\\x0E-\\x1F\\x7F]"));
    
    // 移除多余的空行（保留最多两个连续换行）
    cleaned.replace(QRegularExpression("\\n{3,}"), "\n\n");
    
    // 清理行尾空格
    cleaned.replace(QRegularExpression("[ \\t]+\\n"), "\n");
    
    return cleaned.trimmed();
}

QJsonObject TextParser::extractTextMetadata(const QString& content, const QString& filePath, const QString& encoding) const
{
    QJsonObject metadata;
    QFileInfo fileInfo(filePath);
    
    // 基本文件信息
    metadata["file_path"] = filePath;
    metadata["file_name"] = fileInfo.fileName();
    metadata["file_size"] = fileInfo.size();
    metadata["encoding"] = encoding;
    metadata["parser"] = "TextParser";
    
    // 文本统计
    QStringList lines = content.split('\n');
    metadata["line_count"] = lines.size();
    metadata["character_count"] = content.length();
    metadata["word_count"] = countWords(content);
    metadata["paragraph_count"] = countParagraphs(content);
    
    // 检测文本类型
    metadata["text_type"] = detectTextType(content, fileInfo.suffix());
    
    // 检测语言
    metadata["language"] = detectLanguage(content);
    
    // 添加处理时间
    metadata["processed_time"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    return metadata;
}

int TextParser::countWords(const QString& text) const
{
    // 简单的单词计数
    QStringList words = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    return words.size();
}

int TextParser::countParagraphs(const QString& text) const
{
    QStringList paragraphs = text.split(QRegularExpression("\\n\\s*\\n"), Qt::SkipEmptyParts);
    return paragraphs.size();
}

QString TextParser::detectTextType(const QString& content, const QString& extension) const
{
    QString ext = extension.toLower();
    
    if (ext == "csv") {
        return "CSV";
    }
    if (ext == "tsv") {
        return "TSV";
    }
    if (ext == "log") {
        return "Log";
    }
    
    // 基于内容检测
    if (content.contains(QRegularExpression("^\\d{4}-\\d{2}-\\d{2}.*", QRegularExpression::MultilineOption))) {
        return "Log";
    }
    
    if (content.contains(',') && content.split('\n').size() > 1) {
        QStringList lines = content.split('\n');
        if (lines.size() > 1) {
            int commaCount1 = lines[0].count(',');
            int commaCount2 = lines[1].count(',');
            if (commaCount1 > 0 && commaCount1 == commaCount2) {
                return "CSV";
            }
        }
    }
    
    return "Plain Text";
}

QString TextParser::detectLanguage(const QString& content) const
{
    // 简单的语言检测
    int chineseCount = 0;
    int englishCount = 0;
    
    for (const QChar& ch : content) {
        if (ch.unicode() >= 0x4E00 && ch.unicode() <= 0x9FFF) {
            chineseCount++;
        } else if (ch.isLetter() && ch.unicode() < 128) {
            englishCount++;
        }
    }
    
    if (chineseCount > englishCount) {
        return "Chinese";
    } else if (englishCount > 0) {
        return "English";
    }
    
    return "Unknown";
}
