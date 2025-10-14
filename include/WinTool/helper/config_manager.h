#ifndef WINTOOL_CONFIG_MANAGER_H
#define WINTOOL_CONFIG_MANAGER_H

#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QJsonArray>

class ConfigManager {
public:
    // 单例模式
    static ConfigManager& instance();

    // 通用获取,通过路径获取任意类型的值(支持默认值)
    template<typename T>
    T get(const QString&keyPath, const T&defaultValue = T()) {
        const QJsonValue value = getValue(m_cache, keyPath);
        if (value.isUndefined() || value.isNull()) {
            return defaultValue;
        }

        const QVariant v = value.toVariant();

        // 1. 使用 QMetaType::canConvert 检查是否可转换为目标类型
        const QMetaType targetType = QMetaType::fromType<T>();
        const QMetaType sourceType = v.metaType();

        if (QMetaType::canConvert(sourceType, targetType)) {
            return v.value<T>();
        }

        // 2. 特殊处理：QJsonObject ← QVariantMap
        if constexpr (std::is_same_v<T, QJsonObject>) {
            if (sourceType == QMetaType::fromType<QVariantMap>()) {
                return QJsonObject::fromVariantMap(v.value<QVariantMap>());
            }
            return defaultValue;
        }

        // 3. 特殊处理：QJsonArray ← QVariantList
        if constexpr (std::is_same_v<T, QJsonArray>) {
            if (sourceType == QMetaType::fromType<QVariantList>()) {
                return QJsonArray::fromVariantList(v.value<QVariantList>());
            }
            return defaultValue;
        }

        // 4. 其他情况返回默认值
        return defaultValue;
    }

    // 通用设置,通过路径设置值(自动创建中间结构)
    template<typename T>
    bool set(const QString&keyPath, const T&value) {
        QJsonValue jsonValue;

        // 如果是 C 风格字符串，转为 QString
        if constexpr (std::is_same_v<T, const char *> || std::is_array_v<T>) {
            jsonValue = QString::fromUtf8(value);
        }
        else {
            jsonValue = QJsonValue::fromVariant(QVariant::fromValue(value));
        }

        const QJsonObject newCache = setValue(m_cache, keyPath, jsonValue);
        m_cache = newCache;
        m_dirty = true;
        return save(m_cache); // 自动保存
    }

    // 强制立即保存到文件(通常set后自动保存)
    bool save() const;

    // 获取原始QJsonObject(只读快照)
    QJsonObject toObject() const;

    // 添加服务
    bool addService(const QString& displayName, const QString& serviceName, bool unify);

private:
    // 缓存当前配置(避免频繁磁盘读取)
    mutable QJsonObject m_cache;
    // 是否有为保存的修改
    mutable bool m_dirty{false};
    // 配置文件路径
    mutable QString m_configPath;

    ConfigManager();

    ~ConfigManager();

    // 加载配置
    QJsonObject load() const;

    // 保存QJsonObject对象
    bool save(const QJsonObject&obj) const;

    // 获取配置文件路径
    QString configFilePath() const;

    // 解析路径,支持.和[index]
    static QStringList parsePath(const QString&keyPath);

    // 根据路径获取QJsonValue(核心)
    static QJsonValue getValue(const QJsonObject&root, const QString&keyPath);

    // 根据路径设置QJsonValue(核心)
    static QJsonObject setValue(const QJsonObject&root, const QString&keyPath, const QJsonValue&value);

    // setValue用到的递归
    static QJsonObject setValueRecursive(QJsonObject obj, const QStringList&parts, int index, const QJsonValue&value);
};


#endif //WINTOOL_CONFIG_MANAGER_H
