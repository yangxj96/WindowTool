add_library(helper SHARED
        ConfigManager.cpp ConfigManager.h
)

target_include_directories(helper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(helper PRIVATE Qt::Core)

install(TARGETS helper
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
)
