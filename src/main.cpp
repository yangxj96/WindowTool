#include <QApplication>
#include <QStyleFactory.h>
#include <QDebug>

#include "MainWindow.h"
#include "ConfigManager.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    QApplication::setStyle("Windows");
    MainWindow mw;
    mw.show();
    qDebug() << "Available styles:" << QStyleFactory::keys();

    auto config = ConfigManager::instance().load();

    // 2. 如果是首次运行，设置默认值
    if (config.isEmpty()) {
        config["App/Version"] = "1.0.0";
        config["Window/Width"] = 800;
        config["Window/Height"] = 600;

        QJsonArray users;
        users.append("张三");
        users.append("李四");
        config["Users"] = users;

        // 嵌套对象
        QJsonObject lastUser;
        lastUser["name"] = "张三";
        lastUser["loginTime"] = "2025-04-05";
        config["LastUser"] = lastUser;
    }

    // 3. 修改配置（例如：插入新用户）
    QJsonArray users = config["Users"].toArray();
    users.append("王五"); // 插入新用户
    config["Users"] = users;

    // 4. 保存配置
    if (ConfigManager::instance().save(config)) {
        qDebug() << "配置保存成功！路径：" << ConfigManager::instance().configFilePath();
    } else {
        qWarning() << "配置保存失败！";
    }

    return QApplication::exec();
}
