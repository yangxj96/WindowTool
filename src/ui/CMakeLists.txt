add_library(ui SHARED
        # 主页
        ../../include/WinTool/ui/MainWindow.h
        MainWindow.cpp
        MainWindow.ui
        # 服务
        ../../include/WinTool/ui/ServiceWidget.h
        ServiceWidget.cpp
        ServiceWidget.ui
        # JDK切换
        ../../include/WinTool/ui/JdksWidget.h
        JdksWidget.cpp
        JdksWidget.ui
)

target_include_directories(ui
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# 如果 core 模块依赖 Qt 或其他库
target_link_libraries(ui
        PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        Qt6::Network
)

#install(TARGETS ui
#        RUNTIME DESTINATION .
#        LIBRARY DESTINATION .
#)