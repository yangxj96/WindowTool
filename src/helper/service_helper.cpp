#include "WinTool/helper/service_helper.h"
#include <QDebug>

bool ServiceHelper::query(const QString&serviceName, SERVICE_STATUS&status) {
    auto const hSCManager = OpenSCManager(nullptr, nullptr, GENERIC_READ);
    if (!hSCManager) {
        qWarning() << "OpenSCManager failed:" << GetLastError();
        return false;
    }

    auto const hService = OpenService(hSCManager, reinterpret_cast<LPCWSTR>(serviceName.utf16()), GENERIC_READ);
    if (!hService) {
        CloseServiceHandle(hSCManager);
        return false;
    }

    auto const result = QueryServiceStatus(hService, &status);
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);

    return result ? true : false;
}

bool ServiceHelper::start(const QString&serviceName) {
    auto const hSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!hSCManager) {
        qWarning() << "OpenSCManager failed:" << GetLastError();
        return false;
    }

    auto const hService = OpenService(hSCManager, reinterpret_cast<LPCWSTR>(serviceName.utf16()), SERVICE_START | SERVICE_QUERY_STATUS);
    if (!hService) {
        CloseServiceHandle(hSCManager);
        return false;
    }

    SERVICE_STATUS status;
    if (QueryServiceStatus(hService, &status)) {
        if (status.dwCurrentState == SERVICE_RUNNING) {
            CloseServiceHandle(hService);
            CloseServiceHandle(hSCManager);
            return true; // 已经在运行
        }
    }

    auto const result = ::StartService(hService, 0, nullptr);
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);

    return result ? true : false;
}

bool ServiceHelper::stop(const QString&serviceName) {
    auto const hSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!hSCManager) {
        qWarning() << "OpenSCManager failed:" << GetLastError();
        return false;
    }

    auto const hService = OpenService(hSCManager, reinterpret_cast<LPCWSTR>(serviceName.utf16()), SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (!hService) {
        CloseServiceHandle(hSCManager);
        return false;
    }

    SERVICE_STATUS status;
    if (!QueryServiceStatus(hService, &status)) {
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);
        return false;
    }

    if (status.dwCurrentState == SERVICE_STOPPED) {
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);
        return true; // 已经停止
    }

    auto const result = ControlService(hService, SERVICE_CONTROL_STOP, &status);
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);

    return result ? true : false;
}

bool ServiceHelper::exists(const QString&serviceName) {
    auto const hSCManager = OpenSCManager(nullptr, nullptr, GENERIC_READ);
    if (!hSCManager) {
        qWarning() << "OpenSCManager failed:" << GetLastError();
        return false;
    }

    auto const hService = OpenService(hSCManager, reinterpret_cast<LPCWSTR>(serviceName.utf16()), GENERIC_READ);
    auto const exists = hService != nullptr;

    if (hService) {
        CloseServiceHandle(hService);
    }
    CloseServiceHandle(hSCManager);

    return exists;
}

QString ServiceHelper::statusToString(const DWORD state) {
    // @formatter:off
    switch (state) {
        case SERVICE_STOPPED:         return "🔴 已停止";
        case SERVICE_START_PENDING:   return "🟡 启动中";
        case SERVICE_STOP_PENDING:    return "🟠 停止中";
        case SERVICE_RUNNING:         return "🟢 运行中";
        case SERVICE_CONTINUE_PENDING:return "🔵 恢复中";
        case SERVICE_PAUSE_PENDING:   return "🔶 暂停中";
        case SERVICE_PAUSED:          return "⏸️ 已暂停";
        default:                      return QString("❓ 未知状态 (%1)").arg(state);
    }
    // @formatter:on
}
