cmake_minimum_required(VERSION 4.0)
project(WinTool)

# 尝试加载本地配置
if (EXISTS ${CMAKE_SOURCE_DIR}/config.local.cmake)
    include(${CMAKE_SOURCE_DIR}/config.local.cmake)
endif ()

# 设置 C++ 标准和自动化
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt6
find_package(Qt6 COMPONENTS Core Gui Widgets Network REQUIRED)

include_directories(include)

add_subdirectory(src)

# 补全相关文件
if (WIN32 AND EXISTS "${CMAKE_PREFIX_PATH}/bin/windeployqt.exe")
    install(CODE "
        execute_process(
            COMMAND \"${CMAKE_PREFIX_PATH}/bin/windeployqt.exe\"
                    \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/WinTool.exe\"
            WORKING_DIRECTORY \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}\"
        )
    " COMPONENT Runtime)
endif ()

# 启用 CPack 打包功能
include(CPack)

# 设置生成器为 NSIS
set(CPACK_GENERATOR "NSIS")

# --- 基本信息 ---
set(CPACK_PACKAGE_NAME "WinTool")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VENDOR "Your Company")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A powerful Windows tool based on Qt")

# --- 文件信息 ---
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

# --- 安装目录 ---
set(CPACK_PACKAGE_INSTALL_DIRECTORY "WinTool")  # 安装到 C:\Program Files\WinTool

# --- 图标 ---
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/assets/icon.ico")           # 安装程序图标
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/assets/icon_uninstall.ico")  # 卸载程序图标

# --- 桌面和开始菜单快捷方式 ---
set(CPACK_CREATE_DESKTOP_LINKS "WinTool.exe")
set(CPACK_NSIS_MENU_LINKS "WinTool.exe" "WinTool")  # 在开始菜单创建链接

# --- 其他选项 ---
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)  # 安装前自动卸载旧版本
set(CPACK_NSIS_MODIFY_PATH ON)                      # 可选：是否允许添加到系统 PATH
set(CPACK_NSIS_CONTACT "support@yourcompany.com")   # 联系方式
set(CPACK_PACKAGE_EXECUTABLES "WinTool" "WinTool")   # 主程序入口
