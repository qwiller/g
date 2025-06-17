#pragma once

#include <QString>
#include <QJsonObject>
#include <poppler-document.h>
#include <poppler-page.h>
#include <memory>

/**
 * @brief PDF解析结果
 */
struct PDFParseResult
{
    bool success;           ///< 解析是否成功
    QString errorMessage;   ///< 错误信息
    QString content;        ///< 提取的文本内容
    QJsonObject metadata;   ///< PDF元数据
    int pageCount;          ///< 页面数量
    
    PDFParseResult() : success(false), pageCount(0) {}
};

/**
 * @brief PDF解析器类
 * 
 * 使用Poppler库解析PDF文档，提取文本内容和元数据
 */
class PDFParser
{
public:
    /**
     * @brief 构造函数
     */
    PDFParser();

    /**
     * @brief 析构函数
     */
    ~PDFParser();

    /**
     * @brief 解析PDF文件
     * @param filePath PDF文件路径
     * @return 解析结果
     */
    PDFParseResult parsePDF(const QString& filePath);

    /**
     * @brief 解析指定页面范围的PDF
     * @param filePath PDF文件路径
     * @param startPage 起始页面（从1开始）
     * @param endPage 结束页面（-1表示到最后一页）
     * @return 解析结果
     */
    PDFParseResult parsePDFPages(const QString& filePath, int startPage, int endPage = -1);

    /**
     * @brief 检查PDF文件是否有效
     * @param filePath PDF文件路径
     * @return 是否有效
     */
    bool isValidPDF(const QString& filePath) const;

    /**
     * @brief 获取PDF基本信息
     * @param filePath PDF文件路径
     * @return PDF信息的JSON对象
     */
    QJsonObject getPDFInfo(const QString& filePath) const;

private:
    /**
     * @brief 提取单页文本
     * @param page Poppler页面对象
     * @param pageNumber 页面号
     * @return 页面文本内容
     */
    QString extractPageText(poppler::page* page, int pageNumber) const;

    /**
     * @brief 清理PDF文本
     * @param text 原始文本
     * @return 清理后的文本
     */
    QString cleanPDFText(const QString& text) const;

    /**
     * @brief 提取PDF元数据
     * @param document Poppler文档对象
     * @param filePath 文件路径
     * @return 元数据JSON对象
     */
    QJsonObject extractPDFMetadata(poppler::document* document, const QString& filePath) const;

    /**
     * @brief 检测文本编码并转换
     * @param rawText 原始文本
     * @return 转换后的UTF-8文本
     */
    QString convertToUTF8(const QString& rawText) const;

private:
    mutable QString m_lastError;
};
