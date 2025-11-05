#ifndef WINTOOL_NAVICAT_CLEANUP_H
#define WINTOOL_NAVICAT_CLEANUP_H

#include <QString>
#include <vector>
#include <functional>


class NavicatCleanup {
public:
    // 定义日志回调类型，便于外部处理日志信息
    using LogCallback = std::function<void(const QString&)>;
    using ErrorCallback = std::function<void(const QString&)>;

    // 构造函数，可设置日志回调
    explicit NavicatCleanup(
        LogCallback logCallback = nullptr,
        ErrorCallback errorCallback = nullptr
    );

    // 执行完整的注册表清理逻辑
    void cleanup() const;

private:
    // 执行命令并捕获输出，失败时抛出异常
    static QString runCommandAndCaptureOutput(const QString& program, const QStringList& arguments);

    // 静默执行命令（不捕获输出）
    static void runSilentCommand(const QString& program, const QStringList& arguments);

    // 列出指定注册表路径下的所有子项
    static std::vector<QString> listRegistryKeys(const QString& path);

    // 检查注册表项是否包含某个字符串
    static bool registryKeyContains(const QString& key, const QString& term);

    // 日志回调函数
    LogCallback m_logCallback;

    // 错误回调函数
    ErrorCallback m_errorCallback;

    // 内部日志处理
    void log(const QString& message) const;

    // 内部错误处理
    void error(const QString& message) const;
};


#endif //WINTOOL_NAVICAT_CLEANUP_H
