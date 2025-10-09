#ifndef WINTOOL_SERVICE_HELPER_H
#define WINTOOL_SERVICE_HELPER_H

#include <QJsonObject>
#include <windows.h>
#include <winsvc.h>

class ServiceHelper {
public:
    // 查询服务状态
    static bool query(const QString&serviceName, SERVICE_STATUS&status);

    // 启动服务
    static bool start(const QString&serviceName);

    // 停止服务
    static bool stop(const QString&serviceName);

    // 检查服务是否存在
    static bool exists(const QString&serviceName);

    // 获取状态字符串
    static QString statusToString(DWORD state);
};

// 服务信息
struct ServiceInfo {
    QString display_name;

    QString service_name;

    bool unify;

    // 转换为 QJsonObject
    QJsonObject toJsonObject() const {
        return {
            {"display_name", display_name},
            {"service_name", service_name},
            {"unify", unify},
        };
    }

    // 从 QJsonObject 创建 User
    static ServiceInfo fromJsonObject(const QJsonObject&obj) {
        return ServiceInfo{
            obj["display_name"].toString(),
            obj["service_name"].toString(),
            obj["unify"].toBool()
        };
    }
};

#endif //WINTOOL_SERVICE_HELPER_H
