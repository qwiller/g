#pragma once

#include <QtTest/QtTest>
#include <QObject>

class PDFParser;

/**
 * @brief PDF解析器测试类
 */
class TestPDFParser : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    /**
     * @brief 测试基础功能
     */
    void testBasicFunctionality();

    /**
     * @brief 测试PDF文件有效性检查
     */
    void testValidPDFCheck();

    /**
     * @brief 测试错误处理
     */
    void testErrorHandling();

    /**
     * @brief 测试文本清理
     */
    void testTextCleaning();

    /**
     * @brief 测试编码转换
     */
    void testEncodingConversion();

    /**
     * @brief 测试元数据提取
     */
    void testMetadataExtraction();

private:
    PDFParser* m_parser;
};
