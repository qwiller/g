#pragma once

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSettings>
#include <memory>

/**
 * @brief 配置管理器类
 * 
 * 负责应用程序配置的加载、保存和管理
 */
class ConfigManager
{
public:
    /**
     * @brief 构造函数
     */
    ConfigManager();

    /**
     * @brief 析构函数
     */
    ~ConfigManager();

    /**
     * @brief 加载配置文件
     * @param configPath 配置文件路径
     * @return 加载是否成功
     */
    bool loadConfig(const QString& configPath = "");

    /**
     * @brief 保存配置文件
     * @param configPath 配置文件路径
     * @return 保存是否成功
     */
    bool saveConfig(const QString& configPath = "");

    /**
     * @brief 获取字符串配置值
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    QString getString(const QString& key, const QString& defaultValue = "") const;

    /**
     * @brief 获取整数配置值
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    int getInt(const QString& key, int defaultValue = 0) const;

    /**
     * @brief 获取浮点数配置值
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    double getDouble(const QString& key, double defaultValue = 0.0) const;

    /**
     * @brief 获取布尔配置值
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    bool getBool(const QString& key, bool defaultValue = false) const;

    /**
     * @brief 设置字符串配置值
     * @param key 配置键
     * @param value 配置值
     */
    void setString(const QString& key, const QString& value);

    /**
     * @brief 设置整数配置值
     * @param key 配置键
     * @param value 配置值
     */
    void setInt(const QString& key, int value);

    /**
     * @brief 设置浮点数配置值
     * @param key 配置键
     * @param value 配置值
     */
    void setDouble(const QString& key, double value);

    /**
     * @brief 设置布尔配置值
     * @param key 配置键
     * @param value 配置值
     */
    void setBool(const QString& key, bool value);

    /**
     * @brief 获取完整的JSON配置对象
     * @return JSON配置对象
     */
    QJsonObject getJsonConfig() const;

    /**
     * @brief 检查配置键是否存在
     * @param key 配置键
     * @return 是否存在
     */
    bool hasKey(const QString& key) const;

private:
    /**
     * @brief 从嵌套键获取值
     * @param key 嵌套键（如 "ai.api_endpoint"）
     * @return JSON值
     */
    QJsonValue getNestedValue(const QString& key) const;

    /**
     * @brief 设置嵌套键的值
     * @param key 嵌套键
     * @param value JSON值
     */
    void setNestedValue(const QString& key, const QJsonValue& value);

private:
    QJsonObject m_config;
    QString m_configPath;
    std::unique_ptr<QSettings> m_settings;
};
