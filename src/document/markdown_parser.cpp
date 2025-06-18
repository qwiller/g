#include "document/markdown_parser.h"

#include <QFileInfo>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

MarkdownParser::MarkdownParser() = default;

MarkdownParser::~MarkdownParser() = default;

MarkdownParseResult MarkdownParser::parseMarkdown(const QString& filePath)
{
    MarkdownParseResult result;
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        result.errorMessage = "Markdown文件不存在: " + filePath;
        return result;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        result.errorMessage = "无法打开Markdown文件: " + filePath;
        return result;
    }
    
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    QString content = stream.readAll();
    file.close();
    
    // 解析Markdown内容
    result.content = parseMarkdownContent(content);
    result.metadata = extractMarkdownMetadata(content, filePath);
    result.success = true;
    
    qDebug() << "Markdown解析完成:" << filePath << "内容长度:" << result.content.length();
    
    return result;
}

bool MarkdownParser::isValidMarkdown(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);
    QStringList validExtensions = {"md", "markdown", "mdown", "mkd"};
    return fileInfo.exists() && validExtensions.contains(fileInfo.suffix().toLower());
}

QString MarkdownParser::parseMarkdownContent(const QString& rawContent) const
{
    QString content = rawContent;
    
    // 移除YAML前置元数据
    content = removeYAMLFrontMatter(content);
    
    // 处理标题
    content = processHeaders(content);
    
    // 处理代码块
    content = processCodeBlocks(content);
    
    // 处理链接和图片
    content = processLinksAndImages(content);
    
    // 处理列表
    content = processLists(content);
    
    // 处理强调和粗体
    content = processEmphasis(content);
    
    // 清理多余的空行
    content = content.replace(QRegularExpression("\\n\\s*\\n\\s*\\n"), "\n\n");
    
    return content.trimmed();
}

QString MarkdownParser::removeYAMLFrontMatter(const QString& content) const
{
    QRegularExpression yamlPattern("^---\\s*\\n.*?\\n---\\s*\\n", 
                                  QRegularExpression::DotMatchesEverythingOption);
    return content.mid(yamlPattern.match(content).capturedEnd());
}

QString MarkdownParser::processHeaders(const QString& content) const
{
    QString processed = content;
    
    // ATX风格标题 (# ## ###)
    processed.replace(QRegularExpression("^#{1,6}\\s*(.+)$", QRegularExpression::MultilineOption), 
                     "\\1\n");
    
    // Setext风格标题
    processed.replace(QRegularExpression("^(.+)\\n=+\\s*$", QRegularExpression::MultilineOption), 
                     "\\1\n");
    processed.replace(QRegularExpression("^(.+)\\n-+\\s*$", QRegularExpression::MultilineOption), 
                     "\\1\n");
    
    return processed;
}

QString MarkdownParser::processCodeBlocks(const QString& content) const
{
    QString processed = content;
    
    // 围栏代码块 (```)
    processed.replace(QRegularExpression("```[^\\n]*\\n([^`]*)```", 
                                        QRegularExpression::DotMatchesEverythingOption), 
                     "\n[代码块]\n\\1\n");
    
    // 缩进代码块
    processed.replace(QRegularExpression("^    (.+)$", QRegularExpression::MultilineOption), 
                     "\\1");
    
    // 行内代码
    processed.replace(QRegularExpression("`([^`]+)`"), "\\1");
    
    return processed;
}

QString MarkdownParser::processLinksAndImages(const QString& content) const
{
    QString processed = content;
    
    // 图片 ![alt](url)
    processed.replace(QRegularExpression("!\\[([^\\]]*)\\]\\([^\\)]+\\)"), "[图片: \\1]");
    
    // 链接 [text](url)
    processed.replace(QRegularExpression("\\[([^\\]]+)\\]\\([^\\)]+\\)"), "\\1");
    
    // 自动链接
    processed.replace(QRegularExpression("<(https?://[^>]+)>"), "\\1");
    
    return processed;
}

QString MarkdownParser::processLists(const QString& content) const
{
    QString processed = content;
    
    // 无序列表
    processed.replace(QRegularExpression("^\\s*[-*+]\\s+(.+)$", QRegularExpression::MultilineOption), 
                     "• \\1");
    
    // 有序列表
    processed.replace(QRegularExpression("^\\s*\\d+\\.\\s+(.+)$", QRegularExpression::MultilineOption), 
                     "\\1");
    
    return processed;
}

QString MarkdownParser::processEmphasis(const QString& content) const
{
    QString processed = content;
    
    // 粗体 **text** 或 __text__
    processed.replace(QRegularExpression("\\*\\*([^*]+)\\*\\*"), "\\1");
    processed.replace(QRegularExpression("__([^_]+)__"), "\\1");
    
    // 斜体 *text* 或 _text_
    processed.replace(QRegularExpression("\\*([^*]+)\\*"), "\\1");
    processed.replace(QRegularExpression("_([^_]+)_"), "\\1");
    
    // 删除线 ~~text~~
    processed.replace(QRegularExpression("~~([^~]+)~~"), "\\1");
    
    return processed;
}

QJsonObject MarkdownParser::extractMarkdownMetadata(const QString& content, const QString& filePath) const
{
    QJsonObject metadata;
    QFileInfo fileInfo(filePath);
    
    // 基本文件信息
    metadata["file_path"] = filePath;
    metadata["file_name"] = fileInfo.fileName();
    metadata["file_size"] = fileInfo.size();
    metadata["parser"] = "MarkdownParser";
    
    // 提取YAML前置元数据
    QRegularExpression yamlPattern("^---\\s*\\n(.*?)\\n---\\s*\\n", 
                                  QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch yamlMatch = yamlPattern.match(content);
    
    if (yamlMatch.hasMatch()) {
        QString yamlContent = yamlMatch.captured(1);
        metadata["has_frontmatter"] = true;
        
        // 简单解析YAML键值对
        QStringList lines = yamlContent.split('\n');
        for (const QString& line : lines) {
            QRegularExpression kvPattern("^\\s*([^:]+):\\s*(.+)\\s*$");
            QRegularExpressionMatch kvMatch = kvPattern.match(line);
            if (kvMatch.hasMatch()) {
                QString key = kvMatch.captured(1).trimmed();
                QString value = kvMatch.captured(2).trimmed();
                metadata[key.toLower()] = value;
            }
        }
    } else {
        metadata["has_frontmatter"] = false;
    }
    
    // 统计信息
    QStringList lines = content.split('\n');
    metadata["line_count"] = lines.size();
    
    // 统计标题数量
    int headerCount = 0;
    for (const QString& line : lines) {
        if (line.startsWith('#')) {
            headerCount++;
        }
    }
    metadata["header_count"] = headerCount;
    
    // 添加处理时间
    metadata["processed_time"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    return metadata;
}
