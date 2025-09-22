cmake_minimum_required(VERSION 4.0)
project(WinTool)

# 尝试加载本地配置
if(EXISTS ${CMAKE_SOURCE_DIR}/config.local.cmake)
    include(${CMAKE_SOURCE_DIR}/config.local.cmake)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 放到config.local.cmake文件中了
#set(CMAKE_PREFIX_PATH "D:/Develop/Platform/Qt/6.9.2/mingw_64")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_subdirectory(lib/ui_service)
add_subdirectory(src)

# 配置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

target_link_libraries(WinTool
        Qt::Core
        Qt::Gui
        Qt::Widgets
        ui_service
)

# 将模块头文件包含进来，供 main.cpp 使用
target_include_directories(WinTool PRIVATE
        ui_service
)

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