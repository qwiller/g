#include "test_pdf_parser.h"
#include "document/pdf_parser.h"

#include <QTemporaryFile>
#include <QTextStream>

void TestPDFParser::initTestCase()
{
    qDebug() << "开始PDF解析器测试...";
    m_parser = new PDFParser();
}

void TestPDFParser::cleanupTestCase()
{
    delete m_parser;
    qDebug() << "PDF解析器测试完成";
}

void TestPDFParser::init()
{
    // 每个测试前的准备
}

void TestPDFParser::cleanup()
{
    // 每个测试后的清理
}

void TestPDFParser::testBasicFunctionality()
{
    // 基础功能测试
    QVERIFY(m_parser != nullptr);
    qDebug() << "PDF解析器基础功能测试通过";
}

void TestPDFParser::testValidPDFCheck()
{
    // 测试PDF文件有效性检查
    
    // 测试不存在的文件
    QVERIFY(!m_parser->isValidPDF("nonexistent.pdf"));
    
    // 测试非PDF文件
    QTemporaryFile tempFile;
    tempFile.setFileTemplate("test_XXXXXX.txt");
    QVERIFY(tempFile.open());
    
    QTextStream stream(&tempFile);
    stream << "This is not a PDF file";
    tempFile.close();
    
    QVERIFY(!m_parser->isValidPDF(tempFile.fileName()));
    
    qDebug() << "PDF有效性检查测试通过";
}

void TestPDFParser::testErrorHandling()
{
    // 测试错误处理
    
    // 测试解析不存在的文件
    PDFParseResult result = m_parser->parsePDF("nonexistent.pdf");
    QVERIFY(!result.success);
    QVERIFY(!result.errorMessage.isEmpty());
    
    // 测试解析非PDF文件
    QTemporaryFile tempFile;
    tempFile.setFileTemplate("test_XXXXXX.pdf");
    QVERIFY(tempFile.open());
    
    QTextStream stream(&tempFile);
    stream << "This is not a valid PDF content";
    tempFile.close();
    
    PDFParseResult invalidResult = m_parser->parsePDF(tempFile.fileName());
    QVERIFY(!invalidResult.success);
    QVERIFY(!invalidResult.errorMessage.isEmpty());
    
    qDebug() << "PDF错误处理测试通过";
}

void TestPDFParser::testTextCleaning()
{
    // 测试文本清理功能
    QString dirtyText = "这是一个\r\n测试文本\t包含\r特殊字符  ";
    QString cleanedText = m_parser->cleanPDFText(dirtyText);
    
    // 验证文本已被清理
    QVERIFY(!cleanedText.contains('\r'));
    QVERIFY(!cleanedText.startsWith(' '));
    QVERIFY(!cleanedText.endsWith(' '));
    
    qDebug() << "PDF文本清理测试通过";
}

void TestPDFParser::testEncodingConversion()
{
    // 测试编码转换功能
    QString testText = "测试中文编码转换";
    QString convertedText = m_parser->convertToUTF8(testText);
    
    // 基本验证：转换后的文本不应为空
    QVERIFY(!convertedText.isEmpty());
    
    qDebug() << "PDF编码转换测试通过";
}

void TestPDFParser::testMetadataExtraction()
{
    // 测试元数据提取功能
    
    // 由于我们没有真实的PDF文件，这里只测试函数不会崩溃
    QJsonObject info = m_parser->getPDFInfo("nonexistent.pdf");
    
    // 对于不存在的文件，应该返回空的JSON对象
    QVERIFY(info.isEmpty() || info.contains("error"));
    
    qDebug() << "PDF元数据提取测试通过";
}

#include "test_pdf_parser.moc"
