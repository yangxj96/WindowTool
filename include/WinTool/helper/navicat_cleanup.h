#ifndef WINTOOL_NAVICAT_CLEANUP_H
#define WINTOOL_NAVICAT_CLEANUP_H

#include <QString>
#include <vector>

// 前向声明 Windows HKEY
enum class RegRoot;

class NavicatCleanup {
public:
    NavicatCleanup() = default;
    ~NavicatCleanup() = default;

    NavicatCleanup(const NavicatCleanup&) = delete;
    NavicatCleanup& operator=(const NavicatCleanup&) = delete;

    NavicatCleanup(NavicatCleanup&&) = default;
    NavicatCleanup& operator=(NavicatCleanup&&) = default;

    /**
     * @brief 执行完整的 Navicat 注册表清理
     */
    static void cleanup();

private:
    // 工具函数（使用 QString 与 Qt 交互，内部转为 std::wstring）
    static void deleteRegistryKey(RegRoot root, const QString& subKey);

    static std::vector<QString> listRegistrySubKeys(RegRoot root, const QString& path);

    static bool registryKeyContainsString(RegRoot root, const QString& keyPath, const QString& searchTerm);
};


#endif //WINTOOL_NAVICAT_CLEANUP_H
