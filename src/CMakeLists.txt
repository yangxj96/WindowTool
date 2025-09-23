# 不加 WIN32
add_executable(WinTool main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../resources/application.rc
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
)

# 在 Release 中隐藏控制台
if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set_target_properties(WinTool PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()

# 处理 Windows 平台特定的 DLL 复制
if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)

    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()

    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")

    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()

    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()

    # 复制QT库
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)

    # 复制我们自己的模块到指定位置
    foreach(MODULE ${PROJECT_MODULES})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${MODULE}>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/
                COMMENT "Copying ${MODULE}.dll to output directory"
                VERBATIM
        )
    endforeach()

#    # 复制自定义模块 DLL
#    add_custom_command(TARGET WinTool POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy
#            $<TARGET_FILE:ui_service>
#            "$<TARGET_FILE_DIR:WinTool>/"
#            COMMENT "Copying ui_service.dll..."
#    )
#
#    # 复制自定义模块 DLL
#    add_custom_command(TARGET WinTool POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy
#            $<TARGET_FILE:ui_jdks>
#            "$<TARGET_FILE_DIR:WinTool>/"
#            COMMENT "Copying ui_jdks.dll..."
#    )

endif ()

# 安装主程序
install(TARGETS WinTool RUNTIME DESTINATION .)
