#include "WinTool/helper/config_manager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QTextStream>
#include <QVariant>
#include <QDebug>
#include <QJsonArray>
#include <QVariant>
#include <memory>

ConfigManager& ConfigManager::instance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::save() const {
    return save(m_cache);
}

QJsonObject ConfigManager::toObject() const {
    return m_cache;
}

/***************************** 私有方法区 *****************************/

ConfigManager::ConfigManager() {
    m_cache = load();
}

ConfigManager::~ConfigManager() {
    if (m_dirty) {
        save(m_cache);
    }
}

QJsonObject ConfigManager::load() const {
    const QString filePath = configFilePath();
    QFile file(filePath);

    // 文件不存在->返回空对象,并标记需要首次保存
    if (!file.exists()) {
        qDebug() << "[Config] 配置文件不存在,将创建:" << filePath;
        return {};
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[Config] 无法读取配置文件:" << filePath;
        return {};
    }

    const QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "[Config] JSON解析错误" << error.errorString();
        return {};
    }

    return doc.isObject() ? doc.object() : QJsonObject();
}

bool ConfigManager::save(const QJsonObject&obj) const {
    const QString filePath = configFilePath();
    QFile file(filePath);

    const QFileInfo fileInfo(filePath);
    if (const QDir dir = fileInfo.absoluteDir(); !dir.exists()) {
        (void)dir.mkpath(".");
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qWarning() << "[Config] 无法打开文件以写入：" << filePath;
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << QJsonDocument(obj).toJson(QJsonDocument::Indented);
    stream.flush();
    file.close();

    if (file.error() != QFile::NoError) {
        qWarning() << "[Config] 写入文件失败：" << file.errorString();
        return false;
    }

    m_dirty = false;
    return true;
}

QStringList ConfigManager::parsePath(const QString&keyPath) {
    // 简化版：目前只支持 . 分隔和 [n] 数组访问
    // 示例：users.0.name → ["users", "0", "name"]
    // 更复杂可用正则，这里手动处理
    QStringList result;
    QString temp = keyPath;
    temp.replace("[", ".").replace("]", "");
    return temp.split('.', Qt::SkipEmptyParts);
}

QString ConfigManager::configFilePath() const {
    if (m_configPath.isEmpty()) {
        auto const exeDir = QCoreApplication::applicationDirPath();
        m_configPath = exeDir + "/config.json";
    }
    return m_configPath;
}

QJsonValue ConfigManager::getValue(const QJsonObject&root, const QString&keyPath) {
    QStringList parts = parsePath(keyPath);
    QJsonValue value = root;

    for (const QString&part: parts) {
        if (value.isObject()) {
            value = value.toObject().value(part);
        }
        else if (value.isArray()) {
            bool ok = false;
            int index = part.toInt(&ok);
            if (ok && index >= 0 && index < value.toArray().size()) {
                value = value.toArray().at(index);
            }
            else {
                return QJsonValue(); // 索引越界或无效
            }
        }
        else {
            return QJsonValue(); // 类型不匹配
        }
    }

    return value.isUndefined() ? QJsonValue() : value;
}

QJsonObject ConfigManager::setValue(QJsonObject root, const QString&keyPath, const QJsonValue&value) {
    QStringList parts = parsePath(keyPath);
    return setValueRecursive(root, parts, 0, value);
}

QJsonObject ConfigManager::setValueRecursive(QJsonObject obj, const QStringList&parts, int index, const QJsonValue&value) {
    if (index >= parts.size()) {
        return obj;
    }

    const QString&part = parts[index];
    const bool isLast = (index == parts.size() - 1);

    // 处理数组
    if (part.contains('[')) {
        QString baseName = part.mid(0, part.indexOf('['));
        bool ok = false;
        int arrayIndex = part.mid(part.indexOf('[') + 1, part.indexOf(']') - part.indexOf('[') - 1).toInt(&ok);
        if (!ok) return obj;

        QJsonArray arr;
        if (obj.contains(baseName) && obj[baseName].isArray()) {
            arr = obj[baseName].toArray();
        }

        // 扩展数组
        while (arr.size() <= arrayIndex) {
            arr.append(QJsonValue::Undefined);
        }

        if (isLast) {
            arr[arrayIndex] = value;
        }
        else {
            QJsonObject child;
            if (arrayIndex < arr.size() && arr[arrayIndex].isObject()) {
                child = arr[arrayIndex].toObject();
            }
            QJsonObject newChild = setValueRecursive(child, parts, index + 1, value);
            arr[arrayIndex] = newChild;
        }

        obj[baseName] = arr;
        return obj;
    }

    // 处理对象
    if (!obj.contains(part)) {
        obj[part] = isLast ? value : QJsonObject();
    }

    if (isLast) {
        obj[part] = value;
    }
    else {
        QJsonObject child;
        if (obj[part].isObject()) {
            child = obj[part].toObject();
        }
        QJsonObject newChild = setValueRecursive(child, parts, index + 1, value);
        obj[part] = newChild;
    }

    return obj;
}
