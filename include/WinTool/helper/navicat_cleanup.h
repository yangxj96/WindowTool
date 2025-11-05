#ifndef WINTOOL_NAVICAT_CLEANUP_H
#define WINTOOL_NAVICAT_CLEANUP_H

#include <QString>
#include <vector>
#include <functional>

class NavicatCleanup {
public:
    // 构造函数（无需回调）
    NavicatCleanup();

    // 执行完整的注册表清理逻辑
    bool cleanup();

private:
    // 执行命令并捕获输出，失败时抛出异常
    QString runCommandAndCaptureOutput(const QString& program, const QStringList& arguments);

    // 列出指定注册表路径下的所有子项
    std::vector<QString> listRegistryKeys(const QString& path);

    // 检查注册表项是否包含某个字符串
    bool registryKeyContains(const QString& key, const QString& term);
};


#endif //WINTOOL_NAVICAT_CLEANUP_H
