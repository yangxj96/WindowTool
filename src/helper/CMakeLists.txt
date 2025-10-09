add_library(helper SHARED
        # 主页
        ../../include/WinTool/helper/config_manager.h
        config_manager.cpp
)

target_include_directories(helper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

# 如果 core 模块依赖 Qt 或其他库
target_link_libraries(helper
        PRIVATE
        Qt6::Core
)
