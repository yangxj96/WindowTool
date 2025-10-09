add_library(helper SHARED
        # 配置管理
        ../../include/WinTool/helper/config_manager.h
        config_manager.cpp
        # 服务相关处理
        ../../include/WinTool/helper/service_helper.h
        service_helper.cpp
)

target_include_directories(helper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

# 如果 core 模块依赖 Qt 或其他库
target_link_libraries(helper
        PRIVATE
        Qt6::Core
        # 服务管理
        advapi32
)
