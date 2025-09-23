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
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# 添加子模块（确保它们定义了目标）
add_subdirectory(lib/ui_jdks)
add_subdirectory(lib/ui_service)
add_subdirectory(src)

# 配置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 定义模块
set(PROJECT_MODULES ui_service ui_jdks)

# 链接库（Qt:: 前缀自动处理）
target_link_libraries(${PROJECT_NAME}
        Qt::Core
        Qt::Gui
        Qt::Widgets
        ${PROJECT_MODULES}
)

# 将模块头文件包含进来，供 main.cpp 使用
#target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_MODULES})

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