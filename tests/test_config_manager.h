#pragma once

#include <QtTest/QtTest>
#include <QObject>
#include <QTemporaryFile>
#include <QJsonObject>
#include <QJsonDocument>

#include "core/config_manager.h"

/**
 * @brief 配置管理器测试类
 */
class TestConfigManager : public QObject
{
    Q_OBJECT

private slots:
    /**
     * @brief 测试初始化
     */
    void initTestCase();

    /**
     * @brief 测试清理
     */
    void cleanupTestCase();

    /**
     * @brief 每个测试前的准备
     */
    void init();

    /**
     * @brief 每个测试后的清理
     */
    void cleanup();

    /**
     * @brief 测试配置文件加载
     */
    void testLoadConfig();

    /**
     * @brief 测试配置文件保存
     */
    void testSaveConfig();

    /**
     * @brief 测试字符串配置项
     */
    void testStringConfig();

    /**
     * @brief 测试整数配置项
     */
    void testIntConfig();

    /**
     * @brief 测试浮点数配置项
     */
    void testDoubleConfig();

    /**
     * @brief 测试布尔配置项
     */
    void testBoolConfig();

    /**
     * @brief 测试嵌套配置项
     */
    void testNestedConfig();

    /**
     * @brief 测试配置项存在性检查
     */
    void testHasKey();

    /**
     * @brief 测试无效配置文件处理
     */
    void testInvalidConfig();

    /**
     * @brief 测试默认值
     */
    void testDefaultValues();

private:
    /**
     * @brief 创建测试配置文件
     * @return 临时文件路径
     */
    QString createTestConfigFile();

    /**
     * @brief 创建无效配置文件
     * @return 临时文件路径
     */
    QString createInvalidConfigFile();

private:
    ConfigManager* m_configManager;
    QTemporaryFile* m_tempFile;
};
