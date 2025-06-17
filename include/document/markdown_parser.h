#pragma once

#include <QString>
#include <QJsonObject>
#include <QStringList>
#include <QRegularExpression>

/**
 * @brief Markdown解析结果
 */
struct MarkdownParseResult
{
    bool success;           ///< 解析是否成功
    QString errorMessage;   ///< 错误信息
    QString content;        ///< 提取的纯文本内容
    QString htmlContent;    ///< 转换的HTML内容
    QJsonObject metadata;   ///< 文档元数据
    QStringList headers;    ///< 标题列表
    
    MarkdownParseResult() : success(false) {}
};

/**
 * @brief Markdown解析器类
 * 
 * 解析Markdown文档，提取文本内容、结构信息和元数据
 */
class MarkdownParser
{
public:
    /**
     * @brief 构造函数
     */
    MarkdownParser();

    /**
     * @brief 析构函数
     */
    ~MarkdownParser();

    /**
     * @brief 解析Markdown文件
     * @param filePath Markdown文件路径
     * @return 解析结果
     */
    MarkdownParseResult parseMarkdown(const QString& filePath);

    /**
     * @brief 解析Markdown文本内容
     * @param content Markdown文本内容
     * @param sourceInfo 来源信息
     * @return 解析结果
     */
    MarkdownParseResult parseMarkdownText(const QString& content, const QJsonObject& sourceInfo = QJsonObject());

    /**
     * @brief 检查文件是否为有效的Markdown
     * @param filePath 文件路径
     * @return 是否有效
     */
    bool isValidMarkdown(const QString& filePath) const;

    /**
     * @brief 提取文档大纲
     * @param content Markdown内容
     * @return 大纲结构
     */
    QJsonObject extractOutline(const QString& content) const;

private:
    /**
     * @brief 移除Markdown标记，提取纯文本
     * @param markdown Markdown内容
     * @return 纯文本内容
     */
    QString stripMarkdown(const QString& markdown) const;

    /**
     * @brief 提取标题列表
     * @param content Markdown内容
     * @return 标题列表
     */
    QStringList extractHeaders(const QString& content) const;

    /**
     * @brief 提取前置元数据（Front Matter）
     * @param content Markdown内容
     * @return 元数据对象和剩余内容
     */
    QPair<QJsonObject, QString> extractFrontMatter(const QString& content) const;

    /**
     * @brief 处理代码块
     * @param content Markdown内容
     * @return 处理后的内容
     */
    QString processCodeBlocks(const QString& content) const;

    /**
     * @brief 处理链接和图片
     * @param content Markdown内容
     * @return 处理后的内容
     */
    QString processLinksAndImages(const QString& content) const;

    /**
     * @brief 处理表格
     * @param content Markdown内容
     * @return 处理后的内容
     */
    QString processTables(const QString& content) const;

    /**
     * @brief 清理文本格式
     * @param text 原始文本
     * @return 清理后的文本
     */
    QString cleanText(const QString& text) const;

    /**
     * @brief 生成文档元数据
     * @param content 文档内容
     * @param filePath 文件路径
     * @param frontMatter 前置元数据
     * @return 元数据对象
     */
    QJsonObject generateMetadata(const QString& content, const QString& filePath, const QJsonObject& frontMatter) const;

private:
    // 正则表达式模式
    QRegularExpression m_headerPattern;
    QRegularExpression m_codeBlockPattern;
    QRegularExpression m_inlineCodePattern;
    QRegularExpression m_linkPattern;
    QRegularExpression m_imagePattern;
    QRegularExpression m_boldPattern;
    QRegularExpression m_italicPattern;
    QRegularExpression m_strikethroughPattern;
    QRegularExpression m_tablePattern;
    QRegularExpression m_frontMatterPattern;
    
    mutable QString m_lastError;
};
