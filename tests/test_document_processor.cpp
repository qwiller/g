#include "test_document_processor.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

void TestDocumentProcessor::initTestCase()
{
    qDebug() << "开始文档处理器测试...";
}

void TestDocumentProcessor::cleanupTestCase()
{
    qDebug() << "文档处理器测试完成";
}

void TestDocumentProcessor::init()
{
    m_processor = new DocumentProcessor();
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
}

void TestDocumentProcessor::cleanup()
{
    delete m_processor;
    delete m_tempDir;
}

void TestDocumentProcessor::testInitialization()
{
    // 测试初始化
    QVERIFY(m_processor->initialize(500, 100));
    
    // 测试支持的格式
    QStringList formats = m_processor->getSupportedFormats();
    QVERIFY(formats.contains(".pdf"));
    QVERIFY(formats.contains(".md"));
    QVERIFY(formats.contains(".txt"));
}

void TestDocumentProcessor::testSupportedFormats()
{
    // 测试支持的格式检查
    QVERIFY(m_processor->isSupportedFormat("test.pdf"));
    QVERIFY(m_processor->isSupportedFormat("test.md"));
    QVERIFY(m_processor->isSupportedFormat("test.txt"));
    QVERIFY(m_processor->isSupportedFormat("test.markdown"));
    
    QVERIFY(!m_processor->isSupportedFormat("test.doc"));
    QVERIFY(!m_processor->isSupportedFormat("test.xlsx"));
    QVERIFY(!m_processor->isSupportedFormat("test.unknown"));
}

void TestDocumentProcessor::testProcessText()
{
    // 初始化处理器
    QVERIFY(m_processor->initialize(200, 50));
    
    // 测试简单文本处理
    QString simpleText = "这是一个简单的测试文本。它包含多个句子。用于测试文档处理功能。";
    ProcessResult result = m_processor->processText(simpleText);
    
    verifyProcessResult(result);
    QVERIFY(result.chunks.size() >= 1);
    
    // 测试长文本处理
    QString longText;
    for (int i = 0; i < 100; ++i) {
        longText += QString("这是第%1段测试文本。").arg(i + 1);
        longText += "它用于测试长文本的分块处理功能。";
        longText += "每段都包含足够的内容来触发分块逻辑。\n";
    }
    
    ProcessResult longResult = m_processor->processText(longText);
    verifyProcessResult(longResult);
    QVERIFY(longResult.chunks.size() > 1);
}

void TestDocumentProcessor::testProcessDocument()
{
    // 初始化处理器
    QVERIFY(m_processor->initialize());
    
    // 测试文本文件处理
    QString textContent = "这是一个测试文档。\n包含多行文本。\n用于验证文档处理功能。";
    QString textFile = createTestTextFile(textContent);
    
    ProcessResult textResult = m_processor->processDocument(textFile);
    verifyProcessResult(textResult);
    
    // 测试Markdown文件处理
    QString markdownContent = "# 测试标题\n\n这是一个测试段落。\n\n## 子标题\n\n- 列表项1\n- 列表项2";
    QString markdownFile = createTestMarkdownFile(markdownContent);
    
    ProcessResult mdResult = m_processor->processDocument(markdownFile);
    verifyProcessResult(mdResult);
    
    // 测试不存在的文件
    ProcessResult invalidResult = m_processor->processDocument("nonexistent.txt");
    QVERIFY(!invalidResult.success);
    QVERIFY(!invalidResult.errorMessage.isEmpty());
}

void TestDocumentProcessor::testChunkParameters()
{
    // 测试分块参数设置
    m_processor->setChunkParameters(300, 75);
    QVERIFY(m_processor->initialize());
    
    QString testText;
    for (int i = 0; i < 50; ++i) {
        testText += QString("这是第%1句测试文本，用于验证分块参数的设置是否正确。").arg(i + 1);
    }
    
    ProcessResult result = m_processor->processText(testText);
    verifyProcessResult(result);
    
    // 验证块的大小合理
    for (const auto& chunk : result.chunks) {
        QVERIFY(chunk.tokenCount > 0);
        QVERIFY(chunk.tokenCount <= 400); // 允许一定的误差
    }
}

void TestDocumentProcessor::testErrorHandling()
{
    // 测试未初始化的处理器
    DocumentProcessor uninitializedProcessor;
    ProcessResult result = uninitializedProcessor.processText("测试文本");
    QVERIFY(!result.success);
    
    // 测试空文本处理
    QVERIFY(m_processor->initialize());
    ProcessResult emptyResult = m_processor->processText("");
    QVERIFY(!emptyResult.success);
    
    // 测试不支持的文件格式
    ProcessResult unsupportedResult = m_processor->processDocument("test.unknown");
    QVERIFY(!unsupportedResult.success);
}

void TestDocumentProcessor::testEncodingDetection()
{
    QVERIFY(m_processor->initialize());
    
    // 测试UTF-8编码检测
    QString utf8File = createTestTextFile("UTF-8测试文本", "UTF-8");
    ProcessResult utf8Result = m_processor->processDocument(utf8File);
    verifyProcessResult(utf8Result);
    
    // 注意：在实际环境中，可能需要测试其他编码格式
    // 但在测试环境中，我们主要关注UTF-8的正确处理
}

void TestDocumentProcessor::testTextCleaning()
{
    QVERIFY(m_processor->initialize());
    
    // 测试包含特殊字符的文本
    QString dirtyText = "这是\r\n一个\t包含\r特殊字符\n的测试文本。  ";
    ProcessResult result = m_processor->processText(dirtyText);
    
    verifyProcessResult(result);
    
    // 验证文本已被清理
    for (const auto& chunk : result.chunks) {
        QVERIFY(!chunk.content.contains('\r'));
        QVERIFY(!chunk.content.startsWith(' '));
        QVERIFY(!chunk.content.endsWith(' '));
    }
}

void TestDocumentProcessor::testTokenCounting()
{
    QVERIFY(m_processor->initialize());
    
    // 测试中文Token计数
    QString chineseText = "这是一个中文测试文本";
    ProcessResult chineseResult = m_processor->processText(chineseText);
    verifyProcessResult(chineseResult);
    
    // 测试英文Token计数
    QString englishText = "This is an English test text";
    ProcessResult englishResult = m_processor->processText(englishText);
    verifyProcessResult(englishResult);
    
    // 测试混合文本Token计数
    QString mixedText = "这是一个mixed中英文test文本";
    ProcessResult mixedResult = m_processor->processText(mixedText);
    verifyProcessResult(mixedResult);
    
    // 验证Token计数的合理性
    for (const auto& chunk : chineseResult.chunks) {
        QVERIFY(chunk.tokenCount > 0);
        QVERIFY(chunk.tokenCount <= chunk.content.length());
    }
}

QString TestDocumentProcessor::createTestTextFile(const QString& content, const QString& encoding)
{
    QString fileName = m_tempDir->filePath("test.txt");
    QFile file(fileName);
    
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    
    QTextStream stream(&file);
    if (encoding == "UTF-8") {
        stream.setCodec("UTF-8");
    }
    
    stream << content;
    file.close();
    
    return fileName;
}

QString TestDocumentProcessor::createTestMarkdownFile(const QString& content)
{
    QString fileName = m_tempDir->filePath("test.md");
    QFile file(fileName);
    
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << content;
    file.close();
    
    return fileName;
}

void TestDocumentProcessor::verifyProcessResult(const ProcessResult& result, int expectedChunks)
{
    QVERIFY2(result.success, result.errorMessage.toLocal8Bit().constData());
    QVERIFY(!result.chunks.empty());
    
    if (expectedChunks > 0) {
        QCOMPARE(result.chunks.size(), static_cast<size_t>(expectedChunks));
    }
    
    // 验证每个块的基本属性
    for (const auto& chunk : result.chunks) {
        QVERIFY(!chunk.chunkId.isEmpty());
        QVERIFY(!chunk.content.isEmpty());
        QVERIFY(chunk.tokenCount > 0);
        QVERIFY(!chunk.metadata.isEmpty());
    }
    
    // 验证文档元数据
    QVERIFY(!result.documentMetadata.isEmpty());
}

#include "test_document_processor.moc"
