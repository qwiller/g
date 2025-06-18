#pragma once

#include <QtTest/QtTest>
#include <QObject>
#include <QTemporaryFile>
#include <QTemporaryDir>

#include "document/document_processor.h"

/**
 * @brief 文档处理器测试类
 */
class TestDocumentProcessor : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    /**
     * @brief 测试文档处理器初始化
     */
    void testInitialization();

    /**
     * @brief 测试支持的文件格式检查
     */
    void testSupportedFormats();

    /**
     * @brief 测试文本内容处理
     */
    void testProcessText();

    /**
     * @brief 测试文档文件处理
     */
    void testProcessDocument();

    /**
     * @brief 测试分块参数设置
     */
    void testChunkParameters();

    /**
     * @brief 测试错误处理
     */
    void testErrorHandling();

    /**
     * @brief 测试编码检测
     */
    void testEncodingDetection();

    /**
     * @brief 测试文本清理
     */
    void testTextCleaning();

    /**
     * @brief 测试Token计数
     */
    void testTokenCounting();

private:
    /**
     * @brief 创建测试文本文件
     * @param content 文件内容
     * @param encoding 文件编码
     * @return 文件路径
     */
    QString createTestTextFile(const QString& content, const QString& encoding = "UTF-8");

    /**
     * @brief 创建测试Markdown文件
     * @param content 文件内容
     * @return 文件路径
     */
    QString createTestMarkdownFile(const QString& content);

    /**
     * @brief 验证处理结果
     * @param result 处理结果
     * @param expectedChunks 期望的块数量
     */
    void verifyProcessResult(const ProcessResult& result, int expectedChunks = -1);

private:
    DocumentProcessor* m_processor;
    QTemporaryDir* m_tempDir;
};
