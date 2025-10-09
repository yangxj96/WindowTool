#include "../../include/WinTool/helper/ConfigManager.h"
#include <QCoreApplication>
#include <QDir>
#include <QJsonDocument>
#include <QTextStream>
#include <QDebug>

ConfigManager& ConfigManager::instance() {
    static ConfigManager instance;
    return instance;
}

QString ConfigManager::configFilePath() const {
    if (m_configPath.isEmpty()) {
        // 获取 .exe 所在目录
        QString exeDir = QCoreApplication::applicationDirPath();
        m_configPath = exeDir + "/config.json";
    }
    return m_configPath;
}

QJsonObject ConfigManager::load() {
    QString filePath = configFilePath();
    QFile file(filePath);

    // 如果文件不存在，返回空对象（由调用者决定默认值）
    if (!file.exists()) {
        qDebug() << "配置文件不存在，将创建：" << filePath;
        return QJsonObject();
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法读取配置文件：" << filePath;
        return QJsonObject();
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON 解析错误：" << error.errorString();
        return QJsonObject();
    }

    if (!doc.isObject()) {
        qWarning() << "JSON 根节点不是对象";
        return QJsonObject();
    }

    return doc.object();
}

bool ConfigManager::save(const QJsonObject&config) {
    QString filePath = configFilePath();
    QFile file(filePath);

    // 确保目录存在（支持多级目录，如 config/app.json）
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "无法写入配置文件：" << filePath;
        return false;
    }

    // 使用 UTF-8 编码写入，避免中文乱码
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << QJsonDocument(config).toJson(QJsonDocument::Indented); // 格式化输出
    stream.flush();
    file.close();

    if (file.error() != QFile::NoError) {
        qWarning() << "写入文件时出错：" << file.errorString();
        return false;
    }

    return true;
}
