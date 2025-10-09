#include "WinTool/helper/navicat_cleanup.h"

#include <QDebug>
#include <windows.h>
#include <winreg.h>
#include <iostream>
#include <format>
#include <system_error>
#include <vector>

using namespace Qt::StringLiterals;

// 宽字符串转utf8
static std::string wstring_to_utf8(const std::wstring&wstr) {
    if (wstr.empty()) return {};
    const int size_needed = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.data(),
        static_cast<int>(wstr.size()),
        nullptr,
        0,
        nullptr,
        nullptr
    );
    std::string str(size_needed, 0);
    WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.data(),
        static_cast<int>(wstr.size()),
        str.data(),
        size_needed,
        nullptr,
        nullptr
    );
    return str;
}

// =======================
// 注册表根键枚举
// =======================
enum class RegRoot {
    CURRENT_USER,
    LOCAL_MACHINE,
    CLASSES_ROOT,
    USERS,
    CURRENT_CONFIG
};

constexpr HKEY toHKey(const RegRoot root) noexcept {
    // @formatter:off
    switch (root) {
        case RegRoot::CURRENT_USER:   return HKEY_CURRENT_USER;
        case RegRoot::LOCAL_MACHINE:  return HKEY_LOCAL_MACHINE;
        case RegRoot::CLASSES_ROOT:   return HKEY_CLASSES_ROOT;
        case RegRoot::USERS:          return HKEY_USERS;
        case RegRoot::CURRENT_CONFIG: return HKEY_CURRENT_CONFIG;
    }
    return nullptr;
    // @formatter:on
}

const wchar_t* regRootName(const RegRoot root) noexcept {
    // @formatter:off
    switch (root) {
        case RegRoot::CURRENT_USER:   return L"HKEY_CURRENT_USER";
        case RegRoot::LOCAL_MACHINE:  return L"HKEY_LOCAL_MACHINE";
        case RegRoot::CLASSES_ROOT:   return L"HKEY_CLASSES_ROOT";
        case RegRoot::USERS:          return L"HKEY_USERS";
        case RegRoot::CURRENT_CONFIG: return L"HKEY_CURRENT_CONFIG";
    }
    return L"UNKNOWN";
    // @formatter:on
}

// =======================
// RAII 包装注册表句柄
// =======================
class RegKey {
    HKEY hKey_ = nullptr;

public:
    explicit RegKey(const HKEY root, const std::wstring&subKey, const REGSAM access = KEY_READ) {
        const LONG result = RegOpenKeyExW(root, subKey.c_str(), 0, access, &hKey_);

        if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND) {
            const std::string msg = "无法打开注册表键: "
                                    + wstring_to_utf8(subKey)
                                    + " (错误码: " + std::to_string(result) + ")";
            throw std::system_error(std::error_code(result, std::generic_category()), msg);
        }

        // 键不存在，视为“空打开”，不抛异常
        if (result == ERROR_FILE_NOT_FOUND) {
            hKey_ = nullptr;
        }
    }

    ~RegKey() {
        if (hKey_) {
            RegCloseKey(hKey_);
        }
    }

    RegKey(const RegKey&) = delete;

    RegKey& operator=(const RegKey&) = delete;

    RegKey(RegKey&&other) noexcept : hKey_(other.hKey_) {
        other.hKey_ = nullptr;
    }

    RegKey& operator=(RegKey&&other) noexcept {
        if (this != &other) {
            if (hKey_) RegCloseKey(hKey_);
            hKey_ = other.hKey_;
            other.hKey_ = nullptr;
        }
        return *this;
    }

    explicit operator bool() const noexcept {
        return hKey_ != nullptr;
    }

    HKEY handle() const noexcept {
        return hKey_;
    }
};

// =======================
// 成员函数实现
// =======================

void NavicatCleanup::cleanup() {
    qDebug() << "正在执行 Navicat 注册表清理...";

    // 1. 删除指定路径
    deleteRegistryKey(RegRoot::CURRENT_USER, u"Software\\PremiumSoft\\NavicatPremium\\Registration17XCS"_s);
    deleteRegistryKey(RegRoot::CURRENT_USER, u"Software\\PremiumSoft\\NavicatPremium\\Update"_s);

    // 2. 遍历 CLSID
    const auto clsidPath = u"Software\\Classes\\CLSID"_s;
    const std::vector<QString> searchTerms = {u"Info", u"ShellFolder"};

    for (const auto subKeys = listRegistrySubKeys(RegRoot::CURRENT_USER, clsidPath);
         const auto&keyName: subKeys) {
        QString fullPath = clsidPath + u"\\" + keyName;

        for (const auto&term: searchTerms) {
            if (registryKeyContainsString(RegRoot::CURRENT_USER, fullPath, term)) {
                deleteRegistryKey(RegRoot::CURRENT_USER, fullPath);
                break;
            }
        }
    }

    qDebug() << "完成注册表清理";
}

void NavicatCleanup::deleteRegistryKey(const RegRoot root, const QString&subKey) {
    const std::wstring wSubKey = subKey.toStdWString();
    const HKEY hRoot = toHKey(root);

    if (const LONG result = RegDeleteTreeW(hRoot, wSubKey.c_str()); result == ERROR_SUCCESS) {
        qDebug() << QString::fromWCharArray(regRootName(root)) + "\\" + subKey << "✅ 删除成功";
    }
    else if (result != ERROR_FILE_NOT_FOUND) {
        qDebug() << QString::fromWCharArray(regRootName(root)) + "\\" + subKey << "❌ 删除失败 (错误码:" << result << ")";
    }
}

std::vector<QString> NavicatCleanup::listRegistrySubKeys(const RegRoot root, const QString&path) {
    std::vector<QString> keys;
    const std::wstring wPath = path.toStdWString();
    const RegKey key(toHKey(root), wPath);

    if (!key) {
        qDebug() << "路径不存在，跳过枚举:" << path;
        return keys;
    }

    wchar_t name[256];
    DWORD index = 0;
    LONG result;

    while (true) {
        DWORD nameSize = 255;
        result = RegEnumKeyExW(key.handle(), index, name, &nameSize,
                               nullptr, nullptr, nullptr, nullptr);

        if (result == ERROR_NO_MORE_ITEMS) {
            break;
        }

        if (result == ERROR_SUCCESS) {
            name[nameSize] = L'\0';
            keys.emplace_back(QString::fromWCharArray(name, nameSize));
        }
        // 其他情况（如 ERROR_MORE_DATA）跳过，但仍递增 index
        index++;

        // 防御性：防止意外无限循环
        if (index > 10000) {
            qWarning() << "⚠️ 子键过多，停止枚举:" << path;
            break;
        }
    }

    return keys;
}

bool NavicatCleanup::registryKeyContainsString(const RegRoot root, const QString&keyPath, const QString&searchTerm) {
    const std::wstring wKeyPath = keyPath.toStdWString();
    const std::wstring wSearchTerm = searchTerm.toStdWString();
    const RegKey key(toHKey(root), wKeyPath, KEY_READ);
    if (!key) return false;

    wchar_t valueName[256];
    wchar_t valueData[512];
    DWORD valueNameSize, valueDataSize, valueType;
    DWORD index = 0;

    while (true) {
        valueNameSize = 255;
        valueDataSize = 510; // 留两个字节给 \0

        const LONG result = RegEnumValueW(key.handle(), index++, valueName, &valueNameSize,
                                          nullptr, &valueType, reinterpret_cast<BYTE *>(valueData), &valueDataSize);

        if (result == ERROR_NO_MORE_ITEMS) break;

        if (result == ERROR_SUCCESS) {
            if (valueType == REG_SZ || valueType == REG_EXPAND_SZ) {
                valueData[valueDataSize / sizeof(wchar_t)] = L'\0'; // ✅ null-terminate
                std::wstring_view dataView(valueData);
                if (dataView.find(wSearchTerm) != std::wstring_view::npos) {
                    return true;
                }
            }
        }
    }

    return false;
}
