#ifndef WINTOOL_CONFIG_MANAGER_H
#define WINTOOL_CONFIG_MANAGER_H

#include <QJsonObject>
#include <QString>

class ConfigManager {
public:
    // 单例模式（可选）
    static ConfigManager& instance();

    // 读取整个配置
    QJsonObject load();

    // 保存整个配置
    bool save(const QJsonObject&config);

    // 获取配置文件路径
    QString configFilePath() const;

private:
    ConfigManager() = default;

    mutable QString m_configPath;
};


#endif //WINTOOL_CONFIG_MANAGER_H
