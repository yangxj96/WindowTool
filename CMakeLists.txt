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