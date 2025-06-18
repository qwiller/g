#include <QtTest/QtTest>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

// 包含所有测试类
#include "test_config_manager.h"
#include "test_document_processor.h"
#include "test_pdf_parser.h"
#include "test_markdown_parser.h"
#include "test_text_parser.h"
#include "test_chunk_manager.h"
#include "test_vector_store.h"
#include "test_rag_engine.h"

/**
 * @brief 测试主函数
 * 
 * 运行所有单元测试和集成测试
 */
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // 设置测试环境
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    
    // 创建测试目录
    QDir().mkpath("test_output");
    QDir().mkpath("test_logs");
    
    qDebug() << "开始运行银河麒麟智能问答助手测试套件...";
    qDebug() << "测试目录:" << QDir::currentPath();
    
    int totalFailures = 0;
    
    // 运行配置管理器测试
    {
        TestConfigManager test;
        totalFailures += QTest::qExec(&test, argc, argv);
    }
    
    // 运行文档处理器测试
    {
        TestDocumentProcessor test;
        totalFailures += QTest::qExec(&test, argc, argv);
    }
    
    // 运行PDF解析器测试
    {
        TestPDFParser test;
        totalFailures += QTest::qExec(&test, argc, argv);
    }
    
    // 运行Markdown解析器测试
    {
        TestMarkdownParser test;
        totalFailures += QTest::qExec(&test, argc, argv);
    }
    
    // 运行文本解析器测试
    {
        TestTextParser test;
        totalFailures += QTest::qExec(&test, argc, argv);
    }
    
    // 运行分块管理器测试
    {
        TestChunkManager test;
        totalFailures += QTest::qExec(&test, argc, argv);
    }
    
    // 运行向量存储测试
    {
        TestVectorStore test;
        totalFailures += QTest::qExec(&test, argc, argv);
    }
    
    // 运行RAG引擎测试
    {
        TestRAGEngine test;
        totalFailures += QTest::qExec(&test, argc, argv);
    }
    
    // 输出测试结果
    qDebug() << "========================================";
    if (totalFailures == 0) {
        qDebug() << "所有测试通过! ✓";
    } else {
        qDebug() << "测试失败数量:" << totalFailures;
    }
    qDebug() << "========================================";
    
    return totalFailures;
}
