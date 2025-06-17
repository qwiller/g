#include "core/config_manager.h"

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QStandardPaths>
#include <QDebug>

ConfigManager::ConfigManager()
    : m_settings(std::make_unique<QSettings>())
{
}

ConfigManager::~ConfigManager() = default;

bool ConfigManager::loadConfig(const QString& configPath)
{
    QString filePath = configPath;
    
    // 如果没有指定路径，使用默认路径
    if (filePath.isEmpty()) {
        filePath = "config/app_config.json";
    }
    
    m_configPath = filePath;
    
    QFile file(filePath);
    if (!file.exists()) {
        qWarning() << "配置文件不存在:" << filePath;
        return false;
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "无法打开配置文件:" << filePath << file.errorString();
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qCritical() << "配置文件JSON解析错误:" << parseError.errorString();
        return false;
    }
    
    if (!doc.isObject()) {
        qCritical() << "配置文件根节点不是JSON对象";
        return false;
    }
    
    m_config = doc.object();
    
    qDebug() << "配置文件加载成功:" << filePath;
    return true;
}

bool ConfigManager::saveConfig(const QString& configPath)
{
    QString filePath = configPath.isEmpty() ? m_configPath : configPath;
    
    if (filePath.isEmpty()) {
        qCritical() << "没有指定配置文件路径";
        return false;
    }
    
    // 确保目录存在
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qCritical() << "无法创建配置文件目录:" << dir.absolutePath();
            return false;
        }
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "无法写入配置文件:" << filePath << file.errorString();
        return false;
    }
    
    QJsonDocument doc(m_config);
    QByteArray data = doc.toJson(QJsonDocument::Indented);
    
    qint64 bytesWritten = file.write(data);
    file.close();
    
    if (bytesWritten != data.size()) {
        qCritical() << "配置文件写入不完整";
        return false;
    }
    
    qDebug() << "配置文件保存成功:" << filePath;
    return true;
}

QString ConfigManager::getString(const QString& key, const QString& defaultValue) const
{
    QJsonValue value = getNestedValue(key);
    if (value.isString()) {
        return value.toString();
    }
    return defaultValue;
}

int ConfigManager::getInt(const QString& key, int defaultValue) const
{
    QJsonValue value = getNestedValue(key);
    if (value.isDouble()) {
        return static_cast<int>(value.toDouble());
    }
    return defaultValue;
}

double ConfigManager::getDouble(const QString& key, double defaultValue) const
{
    QJsonValue value = getNestedValue(key);
    if (value.isDouble()) {
        return value.toDouble();
    }
    return defaultValue;
}

bool ConfigManager::getBool(const QString& key, bool defaultValue) const
{
    QJsonValue value = getNestedValue(key);
    if (value.isBool()) {
        return value.toBool();
    }
    return defaultValue;
}

void ConfigManager::setString(const QString& key, const QString& value)
{
    setNestedValue(key, QJsonValue(value));
}

void ConfigManager::setInt(const QString& key, int value)
{
    setNestedValue(key, QJsonValue(value));
}

void ConfigManager::setDouble(const QString& key, double value)
{
    setNestedValue(key, QJsonValue(value));
}

void ConfigManager::setBool(const QString& key, bool value)
{
    setNestedValue(key, QJsonValue(value));
}

QJsonObject ConfigManager::getJsonConfig() const
{
    return m_config;
}

bool ConfigManager::hasKey(const QString& key) const
{
    QJsonValue value = getNestedValue(key);
    return !value.isUndefined();
}

QJsonValue ConfigManager::getNestedValue(const QString& key) const
{
    QStringList keyParts = key.split('.');
    QJsonValue currentValue = m_config;
    
    for (const QString& part : keyParts) {
        if (!currentValue.isObject()) {
            return QJsonValue();
        }
        
        QJsonObject obj = currentValue.toObject();
        if (!obj.contains(part)) {
            return QJsonValue();
        }
        
        currentValue = obj[part];
    }
    
    return currentValue;
}

void ConfigManager::setNestedValue(const QString& key, const QJsonValue& value)
{
    QStringList keyParts = key.split('.');
    if (keyParts.isEmpty()) {
        return;
    }
    
    // 如果只有一个键，直接设置
    if (keyParts.size() == 1) {
        m_config[keyParts[0]] = value;
        return;
    }
    
    // 处理嵌套键
    QJsonObject* currentObj = &m_config;
    
    for (int i = 0; i < keyParts.size() - 1; ++i) {
        const QString& part = keyParts[i];
        
        if (!currentObj->contains(part)) {
            (*currentObj)[part] = QJsonObject();
        }
        
        QJsonValue& currentValue = (*currentObj)[part];
        if (!currentValue.isObject()) {
            currentValue = QJsonObject();
        }
        
        QJsonObject obj = currentValue.toObject();
        (*currentObj)[part] = obj;
        currentObj = &obj;
    }
    
    // 设置最终值
    (*currentObj)[keyParts.last()] = value;
    
    // 需要重新构建整个对象层次结构
    rebuildNestedObject(keyParts, value);
}

void ConfigManager::rebuildNestedObject(const QStringList& keyParts, const QJsonValue& value)
{
    if (keyParts.isEmpty()) {
        return;
    }
    
    // 递归构建嵌套对象
    std::function<QJsonObject(const QStringList&, int, const QJsonValue&)> buildObject = 
        [&](const QStringList& parts, int index, const QJsonValue& val) -> QJsonObject {
        
        QJsonObject obj;
        
        if (index == parts.size() - 1) {
            // 最后一个键，设置值
            obj[parts[index]] = val;
        } else {
            // 中间键，递归构建
            obj[parts[index]] = buildObject(parts, index + 1, val);
        }
        
        return obj;
    };
    
    // 获取现有的根对象
    QJsonObject rootObj = m_config;
    
    // 合并新的嵌套对象
    QJsonObject newObj = buildObject(keyParts, 0, value);
    mergeJsonObjects(rootObj, newObj);
    
    m_config = rootObj;
}

void ConfigManager::mergeJsonObjects(QJsonObject& target, const QJsonObject& source)
{
    for (auto it = source.begin(); it != source.end(); ++it) {
        const QString& key = it.key();
        const QJsonValue& sourceValue = it.value();
        
        if (target.contains(key) && target[key].isObject() && sourceValue.isObject()) {
            // 递归合并对象
            QJsonObject targetObj = target[key].toObject();
            QJsonObject sourceObj = sourceValue.toObject();
            mergeJsonObjects(targetObj, sourceObj);
            target[key] = targetObj;
        } else {
            // 直接覆盖或添加
            target[key] = sourceValue;
        }
    }
}
