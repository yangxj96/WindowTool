add_library(ui SHARED
        # 主页
        ../../include/WinTool/ui/main_window.h
        main_window.cpp
        main_window.ui
        # 服务
        ../../include/WinTool/ui/service_widget.h
        service_widget.cpp
        service_widget.ui
        # JDK切换
        ../../include/WinTool/ui/jdks_widget.h
        jdks_widget.cpp
        jdks_widget.ui
        # 服务编辑
)

target_include_directories(ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

# 如果 core 模块依赖 Qt 或其他库
target_link_libraries(ui
        PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        Qt6::Network
        helper
)
