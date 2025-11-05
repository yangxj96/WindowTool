#include "WinTool/helper/navicat_cleanup.h"

#include <QProcess>
#include <QStringList>
#include <QDebug>

NavicatCleanup::NavicatCleanup(LogCallback logCallback, ErrorCallback errorCallback)
    : m_logCallback(std::move(logCallback)),
      m_errorCallback(std::move(errorCallback)) {
}

void NavicatCleanup::cleanup() const {
    log("⏳ 正在执行Navicat注册表清理...");

    // 注册表删除操作封装
    auto regDelete = [](const QString&path) {
        runSilentCommand("reg", {"delete", path, "/f"});
    };

    // 删除已知的Navicat注册表项
    regDelete(R"(HKEY_CURRENT_USER\Software\PremiumSoft\NavicatPremium\Registration17XCS)");
    regDelete(R"(HKEY_CURRENT_USER\Software\PremiumSoft\NavicatPremium\Update)");

    // 清理CLSID下包含特定关键词的项
    const QString clsidPath = R"(HKEY_CURRENT_USER\Software\Classes\CLSID)";
    const std::vector<QString> searchTerms = {"Info", "ShellFolder"};

    try {
        for (const std::vector<QString> clsidKeys = listRegistryKeys(clsidPath); const QString&key: clsidKeys) {
            for (const QString&term: searchTerms) {
                if (registryKeyContains(key, term)) {
                    regDelete(key);
                    log(QString("已删除注册表项: %1").arg(key));
                }
            }
        }
    }
    catch (const QString&e) {
        error("❌ 读取注册表键失败: " + e);
    }

    log("✅ 完成注册表清理");
}

QString NavicatCleanup::runCommandAndCaptureOutput(const QString&program, const QStringList&arguments) {
    QProcess process;
    process.start(program, arguments);

    // 等待命令执行完成（超时设为5秒）
    if (!process.waitForFinished(5000)) {
        // 抛出std::runtime_error，错误信息从QString转换为UTF-8字符串
        throw std::runtime_error(
            QString("命令执行超时或失败: %1").arg(process.errorString()).toUtf8().constData()
        );
    }

    // 检查命令退出码
    if (process.exitCode() != 0) {
        throw std::runtime_error(
            QString("命令执行失败 (Exit Code: %1)").arg(process.exitCode()).toUtf8().constData()
        );
    }

    // 返回标准输出（转换为UTF-8字符串）
    return QString::fromUtf8(process.readAllStandardOutput());
}

void NavicatCleanup::runSilentCommand(const QString&program, const QStringList&arguments) {
    QProcess process;
    process.start(program, arguments);
    // 静默执行无需捕获输出，等待完成即可（超时设为3秒）
    process.waitForFinished(3000);
}

std::vector<QString> NavicatCleanup::listRegistryKeys(const QString&path) {
    const QString output = runCommandAndCaptureOutput("reg", {"query", path});
    std::vector<QString> keys;

    // 按行分割输出并过滤有效的注册表项
    for (QStringList lines = output.split('\n', Qt::SkipEmptyParts); const QString&line: lines) {
        // 注册表项以"HKEY_"开头
        if (QString trimmedLine = line.trimmed(); trimmedLine.startsWith("HKEY_")) {
            keys.push_back(trimmedLine);
        }
    }

    return keys;
}

bool NavicatCleanup::registryKeyContains(const QString&key, const QString&term) {
    try {
        // 搜索注册表项及其子项中包含的关键词
        const QString output = runCommandAndCaptureOutput("reg", {"query", key, "/s", "/f", term});
        return output.contains(term, Qt::CaseInsensitive);
    }
    catch (const QString&) {
        // 命令执行失败（如项不存在）视为不包含
        return false;
    }
}

void NavicatCleanup::log(const QString&message) const {
    if (m_logCallback) {
        m_logCallback(message);
    }
    else {
        // 默认输出到调试控制台
        qDebug().noquote() << message;
    }
}

void NavicatCleanup::error(const QString&message) const {
    if (m_errorCallback) {
        m_errorCallback(message);
    }
    else {
        // 默认输出到警告控制台
        qWarning().noquote() << message;
    }
}
