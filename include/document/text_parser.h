#pragma once

#include <QString>
#include <QJsonObject>
#include <QTextCodec>

/**
 * @brief 文本解析结果
 */
struct TextParseResult
{
    bool success;           ///< 解析是否成功
    QString errorMessage;   ///< 错误信息
    QString content;        ///< 文本内容
    QJsonObject metadata;   ///< 文档元数据
    QString encoding;       ///< 检测到的编码
    
    TextParseResult() : success(false) {}
};

/**
 * @brief 文本解析器类
 * 
 * 处理纯文本文件，包括编码检测、内容清理和元数据提取
 */
class TextParser
{
public:
    /**
     * @brief 构造函数
     */
    TextParser();

    /**
     * @brief 析构函数
     */
    ~TextParser();

    /**
     * @brief 解析文本文件
     * @param filePath 文本文件路径
     * @return 解析结果
     */
    TextParseResult parseTextFile(const QString& filePath);

    /**
     * @brief 解析文本内容
     * @param content 文本内容
     * @param sourceInfo 来源信息
     * @return 解析结果
     */
    TextParseResult parseTextContent(const QString& content, const QJsonObject& sourceInfo = QJsonObject());

    /**
     * @brief 检测文本编码
     * @param filePath 文件路径
     * @return 检测到的编码名称
     */
    QString detectEncoding(const QString& filePath) const;

    /**
     * @brief 检测文本编码（从字节数据）
     * @param data 字节数据
     * @return 检测到的编码名称
     */
    QString detectEncodingFromData(const QByteArray& data) const;

    /**
     * @brief 转换文本编码到UTF-8
     * @param data 原始字节数据
     * @param encoding 源编码
     * @return UTF-8编码的文本
     */
    QString convertToUTF8(const QByteArray& data, const QString& encoding) const;

    /**
     * @brief 检查文件是否为有效的文本文件
     * @param filePath 文件路径
     * @return 是否有效
     */
    bool isValidTextFile(const QString& filePath) const;

private:
    /**
     * @brief 清理文本内容
     * @param text 原始文本
     * @return 清理后的文本
     */
    QString cleanText(const QString& text) const;

    /**
     * @brief 标准化换行符
     * @param text 原始文本
     * @return 标准化后的文本
     */
    QString normalizeLineEndings(const QString& text) const;

    /**
     * @brief 移除控制字符
     * @param text 原始文本
     * @return 清理后的文本
     */
    QString removeControlCharacters(const QString& text) const;

    /**
     * @brief 生成文本元数据
     * @param content 文本内容
     * @param filePath 文件路径
     * @param encoding 编码格式
     * @return 元数据对象
     */
    QJsonObject generateMetadata(const QString& content, const QString& filePath, const QString& encoding) const;

    /**
     * @brief 分析文本统计信息
     * @param content 文本内容
     * @return 统计信息对象
     */
    QJsonObject analyzeTextStatistics(const QString& content) const;

    /**
     * @brief 检测文本语言
     * @param content 文本内容
     * @return 语言代码
     */
    QString detectLanguage(const QString& content) const;

    /**
     * @brief 使用BOM检测编码
     * @param data 字节数据
     * @return 编码名称，如果没有BOM则返回空字符串
     */
    QString detectBOM(const QByteArray& data) const;

    /**
     * @brief 使用启发式方法检测编码
     * @param data 字节数据
     * @return 最可能的编码名称
     */
    QString heuristicEncodingDetection(const QByteArray& data) const;

private:
    mutable QString m_lastError;
    
    // 支持的编码列表
    QStringList m_supportedEncodings;
};
