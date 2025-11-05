add_library(WtUi SHARED
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
        # 杂项
        ../../include/WinTool/ui/misc.h
        misc.cpp
        misc.ui
)

target_include_directories(WtUi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

# 如果 core 模块依赖 Qt 或其他库
target_link_libraries(WtUi
        PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        Qt6::Network
        WtHelper
)
