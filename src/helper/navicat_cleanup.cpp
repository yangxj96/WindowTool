#include "WinTool/helper/navicat_cleanup.h"

#include <QProcess>
#include <QStringList>
#include <QDebug>
#include <stdexcept>
#include <string>


NavicatCleanup::NavicatCleanup() {
}

// 执行清理并返回结果（true表示成功，false表示失败）
bool NavicatCleanup::cleanup() {
    qDebug().noquote() << "正在执行Navicat注册表清理...";

    bool isSuccess = true;
    QString errorDetails;

    // 注册表删除操作
    auto regDelete = [&](const QString&path) {
        QProcess process;
        process.start("reg", {"delete", path, "/f"});
        if (!process.waitForFinished(3000)) {
            isSuccess = false;
            errorDetails += QString("删除注册表项 %1 超时: %2\n")
                    .arg(path)
                    .arg(process.errorString());
            qWarning().noquote() << errorDetails.trimmed();
        }
        else if (process.exitCode() != 0) {
            // 非0退出码但可能是正常情况（项不存在），仅记录不标记失败
            qDebug().noquote() << "注册表项不存在或已删除:" << path;
        }
    };

    // 删除已知的Navicat注册表项
    regDelete(R"(HKEY_CURRENT_USER\Software\PremiumSoft\NavicatPremium\Registration17XCS)");
    regDelete(R"(HKEY_CURRENT_USER\Software\PremiumSoft\NavicatPremium\Update)");

    // 清理CLSID下包含特定关键词的项
    const QString clsidPath = R"(HKEY_CURRENT_USER\Software\Classes\CLSID)";
    const std::vector<QString> searchTerms = {"Info", "ShellFolder"};

    try {
        std::vector<QString> clsidKeys = listRegistryKeys(clsidPath);
        qDebug().noquote() << "找到" << clsidKeys.size() << "个CLSID子项，开始检查...";

        for (const QString&key: clsidKeys) {
            for (const QString&term: searchTerms) {
                if (registryKeyContains(key, term)) {
                    regDelete(key);
                    qDebug().noquote() << "已删除注册表项:" << key;
                }
            }
        }
    }
    catch (const std::exception&e) {
        isSuccess = false;
        errorDetails += "读取注册表键失败: " + QString::fromLocal8Bit(e.what()) + "\n";
        qWarning().noquote() << errorDetails.trimmed();
    }

    if (isSuccess) {
        qDebug().noquote() << "注册表清理完成";
    }
    else {
        qWarning().noquote() << "注册表清理完成但存在错误";
    }

    return isSuccess;
}

QString NavicatCleanup::runCommandAndCaptureOutput(const QString&program, const QStringList&arguments) {
    QProcess process;
    process.start(program, arguments);

    if (!process.waitForFinished(5000)) {
        // 错误信息编码转换：GBK → UTF-8
        QByteArray errorBytes = process.errorString().toLocal8Bit(); // 获取本地编码（GBK）
        QString errorStr = QString::fromLocal8Bit(errorBytes); // 转换为UTF-8
        throw std::runtime_error(
            QString("命令执行超时或失败: %1").arg(errorStr).toUtf8().constData()
        );
    }

    if (process.exitCode() != 0) {
        // 读取命令的标准错误输出（reg命令的错误信息是GBK编码）
        QByteArray errOutput = process.readAllStandardError();
        QString errStr = QString::fromLocal8Bit(errOutput); // 强制用本地编码（GBK）解析
        throw std::runtime_error(
            QString("命令执行失败 (Exit Code: %1): %2")
                .arg(process.exitCode())
                .arg(errStr)
                .toUtf8()
                .constData()
        );
    }

    // 注册表内容输出同样用GBK解析
    QByteArray outputBytes = process.readAllStandardOutput();
    return QString::fromLocal8Bit(outputBytes);
}

std::vector<QString> NavicatCleanup::listRegistryKeys(const QString&path) {
    QString output = runCommandAndCaptureOutput("reg", {"query", path});
    std::vector<QString> keys;

    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    for (const QString&line: lines) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.startsWith("HKEY_")) {
            keys.push_back(trimmedLine);
        }
    }

    return keys;
}

bool NavicatCleanup::registryKeyContains(const QString&key, const QString&term) {
    try {
        QString output = runCommandAndCaptureOutput("reg", {"query", key, "/s", "/f", term});
        return output.contains(term, Qt::CaseInsensitive);
    }
    catch (const std::exception&e) {
        qWarning().noquote() << "检查注册表项" << key << "时出错:" << e.what();
        return false;
    }
}
