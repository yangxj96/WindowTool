//
// Created by admin on 2025/9/23.
//

#ifndef WINTOOL_SERVICE_INFO_H
#define WINTOOL_SERVICE_INFO_H

#include <QVariant>

struct ServiceInfo {
    QString name;

    int age;

    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["name"] = name;
        map["age"] = age;
        return map;
    }

    static ServiceInfo formVariantMap(const QVariantMap&map) {
        ServiceInfo si;
        si.name = map["name"].toString();
        si.age = map["age"].toInt();
        return si;
    }
};


#endif //WINTOOL_SERVICE_INFO_H
