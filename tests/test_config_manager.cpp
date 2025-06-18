#include "test_config_manager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>

void TestConfigManager::initTestCase()
{
    // 测试套件初始化
    qDebug() << "开始配置管理器测试...";
}

void TestConfigManager::cleanupTestCase()
{
    // 测试套件清理
    qDebug() << "配置管理器测试完成";
}

void TestConfigManager::init()
{
    // 每个测试前的初始化
    m_configManager = new ConfigManager();
    m_tempFile = nullptr;
}

void TestConfigManager::cleanup()
{
    // 每个测试后的清理
    delete m_configManager;
    if (m_tempFile) {
        delete m_tempFile;
    }
}

void TestConfigManager::testLoadConfig()
{
    // 测试配置文件加载
    QString configFile = createTestConfigFile();
    
    QVERIFY(m_configManager->loadConfig(configFile));
    
    // 验证加载的配置
    QCOMPARE(m_configManager->getString("application.name"), QString("测试应用"));
    QCOMPARE(m_configManager->getInt("application.version"), 1);
    QCOMPARE(m_configManager->getBool("debug.enabled"), true);
}

void TestConfigManager::testSaveConfig()
{
    // 测试配置文件保存
    m_tempFile = new QTemporaryFile();
    QVERIFY(m_tempFile->open());
    QString configFile = m_tempFile->fileName();
    m_tempFile->close();
    
    // 设置一些配置
    m_configManager->setString("test.name", "保存测试");
    m_configManager->setInt("test.value", 42);
    m_configManager->setBool("test.flag", true);
    
    // 保存配置
    QVERIFY(m_configManager->saveConfig(configFile));
    
    // 创建新的配置管理器并加载
    ConfigManager newManager;
    QVERIFY(newManager.loadConfig(configFile));
    
    // 验证保存的配置
    QCOMPARE(newManager.getString("test.name"), QString("保存测试"));
    QCOMPARE(newManager.getInt("test.value"), 42);
    QCOMPARE(newManager.getBool("test.flag"), true);
}

void TestConfigManager::testStringConfig()
{
    // 测试字符串配置项
    QString testValue = "测试字符串";
    m_configManager->setString("test.string", testValue);
    
    QCOMPARE(m_configManager->getString("test.string"), testValue);
    QCOMPARE(m_configManager->getString("nonexistent.key", "默认值"), QString("默认值"));
}

void TestConfigManager::testIntConfig()
{
    // 测试整数配置项
    int testValue = 12345;
    m_configManager->setInt("test.integer", testValue);
    
    QCOMPARE(m_configManager->getInt("test.integer"), testValue);
    QCOMPARE(m_configManager->getInt("nonexistent.key", 999), 999);
}

void TestConfigManager::testDoubleConfig()
{
    // 测试浮点数配置项
    double testValue = 3.14159;
    m_configManager->setDouble("test.double", testValue);
    
    QCOMPARE(m_configManager->getDouble("test.double"), testValue);
    QCOMPARE(m_configManager->getDouble("nonexistent.key", 2.718), 2.718);
}

void TestConfigManager::testBoolConfig()
{
    // 测试布尔配置项
    m_configManager->setBool("test.bool_true", true);
    m_configManager->setBool("test.bool_false", false);
    
    QCOMPARE(m_configManager->getBool("test.bool_true"), true);
    QCOMPARE(m_configManager->getBool("test.bool_false"), false);
    QCOMPARE(m_configManager->getBool("nonexistent.key", true), true);
}

void TestConfigManager::testNestedConfig()
{
    // 测试嵌套配置项
    m_configManager->setString("level1.level2.level3.value", "嵌套值");
    m_configManager->setInt("level1.level2.number", 100);
    
    QCOMPARE(m_configManager->getString("level1.level2.level3.value"), QString("嵌套值"));
    QCOMPARE(m_configManager->getInt("level1.level2.number"), 100);
}

void TestConfigManager::testHasKey()
{
    // 测试配置项存在性检查
    m_configManager->setString("existing.key", "存在的键");
    
    QVERIFY(m_configManager->hasKey("existing.key"));
    QVERIFY(!m_configManager->hasKey("nonexistent.key"));
}

void TestConfigManager::testInvalidConfig()
{
    // 测试无效配置文件处理
    QString invalidFile = createInvalidConfigFile();
    
    QVERIFY(!m_configManager->loadConfig(invalidFile));
    
    // 测试不存在的文件
    QVERIFY(!m_configManager->loadConfig("nonexistent_file.json"));
}

void TestConfigManager::testDefaultValues()
{
    // 测试默认值返回
    QCOMPARE(m_configManager->getString("undefined.string", "默认字符串"), QString("默认字符串"));
    QCOMPARE(m_configManager->getInt("undefined.int", 42), 42);
    QCOMPARE(m_configManager->getDouble("undefined.double", 3.14), 3.14);
    QCOMPARE(m_configManager->getBool("undefined.bool", true), true);
}

QString TestConfigManager::createTestConfigFile()
{
    m_tempFile = new QTemporaryFile();
    QVERIFY(m_tempFile->open());
    
    QJsonObject config;
    QJsonObject application;
    application["name"] = "测试应用";
    application["version"] = 1;
    
    QJsonObject debug;
    debug["enabled"] = true;
    debug["level"] = "info";
    
    config["application"] = application;
    config["debug"] = debug;
    
    QJsonDocument doc(config);
    QTextStream stream(m_tempFile);
    stream << doc.toJson();
    
    QString fileName = m_tempFile->fileName();
    m_tempFile->close();
    
    return fileName;
}

QString TestConfigManager::createInvalidConfigFile()
{
    QTemporaryFile* invalidFile = new QTemporaryFile();
    QVERIFY(invalidFile->open());
    
    QTextStream stream(invalidFile);
    stream << "{ invalid json content";
    
    QString fileName = invalidFile->fileName();
    invalidFile->close();
    
    return fileName;
}

#include "test_config_manager.moc"
