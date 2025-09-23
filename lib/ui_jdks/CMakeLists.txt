add_library(ui_jdks SHARED
        JdksWidget.cpp JdksWidget.h
)

target_include_directories(ui_jdks PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ui_jdks PRIVATE Qt::Core Qt::Gui Qt::Widgets)

install(TARGETS ui_jdks
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
)
